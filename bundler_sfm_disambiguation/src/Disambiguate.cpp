/* 
 *  Copyright (c) 2015-2018 Qingan Yan (yanqingan (at) whu.edu.cn　&& 
 *  yanqinganssg (at) gmail.com) and Wuhan UNiversity.
 *
 *  Note that this code [1] is partly based on "bundler_sfm" [2]. 
 *  If you use it for your research, please cite the following papers:
 *  
 *  [1] Qingan Yan, Long Yang, Ling Zhang and Chunxia Xiao.
 *  Distinguishing the Indistinguishable: Exploring Structural Ambiguities
 *  via Geodesic Context. CVPR 2017.
 *  [2] Noah Snavely, Steven M. Seitz, and Richard Szeliski. 
 *  Photo Tourism: Exploring Photo Collections in 3D. SIGGRAPH 2006. 
 *
 *  For more information, please move to my homepage
 *  https://yanqingan.github.io/ or github https://github.com/yanqingan
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 */

/* Disambiguate.cpp */
/* Code for correct ambiguous tracks into distinct ones */

#include <queue>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <vector>

#ifndef WIN32
#include <ext/hash_set>
#include <ext/hash_map>
#else
#include <hash_set>
#include <hash_map>
#endif

#include "keys.h"

#include "BundlerApp.h"
#include "BundleUtil.h"

typedef std::vector<int> IntVect;
/* Return the intersection and non-intersection of two int vectors */	       
std::vector<IntVect> GetVectorItsectAndNonitsect(const std::vector<int> &vs, const std::vector<int> &vt)
{
#ifndef WIN32
	__gnu_cxx::hash_set<int> seen1;
#else
	stdext::hash_set<int> seen1;
#endif

	int ns = (int) vs.size();
	int nt = (int) vt.size();

	std::vector<IntVect> vect_set;
	IntVect itsect;
	IntVect nonitsect;

	for (int i = 0; i < ns; i++)
		seen1.insert(vs[i]);

	for (int i = 0; i < nt; i++) 
	{
		if (seen1.find(vt[i]) != seen1.end())
			itsect.push_back(vt[i]);
		else
			nonitsect.push_back(vt[i]);
	}
	seen1.clear();

	vect_set.push_back(itsect);
	vect_set.push_back(nonitsect);
	return vect_set;
}

bool MyCompareFirst(const KeypointMatch &k1, const KeypointMatch &k2) {
    return (k1.m_idx1 < k2.m_idx1);
}

/* Get required tracks */
void BundlerApp::ComputeConstraintTracks(int new_image_start, int track_degree) 
{
    unsigned int num_images = GetNumImages();

    /* Clear all marks for new images */
    for (unsigned int i = 0; i < num_images; i++) {
        /* If this image has no neighbors, don't worry about its keys */
        // std::list<unsigned int> &nbrs = m_matches.GetNeighbors(i);
        // int num_nbrs = (int) nbrs.size();
        int num_nbrs = (int) m_matches.GetNumNeighbors(i);

        if (num_nbrs == 0)
            continue;

        // m_image_data[i].LoadKeys(false);
	// int num_features = GetNumKeys(i);
        int num_features = m_image_data[i].GetNumKeys();
        m_image_data[i].m_key_flags.resize(num_features);

	// for (int j = 0; j < num_features; j++) {
	//     GetKey(i,j).m_extra = -1;
	// }
    }

    int pt_idx = 0;

    std::vector<TrackData> tracks;

    // KeymatchHashHash hashes;
    // CreateKeymatchHashes(hashes);

    /* Sort all match lists */
#if 0
    for (int i = 0; i < num_images; i++) {
        for (int j = 0; j < num_images; j++) {
            if (i == j)
                continue;

            if (!ImagesMatch(i,j))
                continue;

            MatchIndex idx = GetMatchIndex(i, j);

            std::vector<KeypointMatch> &list = m_match_lists[idx];

            sort(list.begin(), list.end(), MyCompareFirst);
        }
    }
#else
    for (unsigned int i = 0; i < num_images; i++) {
        MatchAdjList::iterator iter;
        for (iter = m_matches.Begin(i); iter != m_matches.End(i); iter++) {
            // MatchIndex idx = *iter;
            std::vector<KeypointMatch> &list = iter->m_match_list; // iter->second; // m_match_lists[idx];
            sort(list.begin(), list.end(), MyCompareFirst);
        }
    }
#endif

    bool *img_marked = new bool[num_images];
    memset(img_marked, 0, num_images * sizeof(bool));

    std::vector<int> touched;
    touched.reserve(num_images);

    for (unsigned int i = 0; i < num_images; i++) {
	int num_features = m_image_data[i].GetNumKeys(); // GetNumKeys(i);

        /* If this image has no neighbors, skip it */
        // std::list<unsigned int> &nbrs = m_matches.GetNeighbors(i);
        // int num_nbrs = (int) nbrs.size();
        int num_nbrs = (int) m_matches.GetNumNeighbors(i);

        if (num_nbrs == 0)
            continue;

	for (int j = 0; j < num_features; j++) {
	    ImageKeyVector features;
	    std::queue<ImageKey> features_queue;

	    /* Check if this feature was visited */
	    // if (GetKey(i,j).m_extra >= 0)
            //      continue;
            if (m_image_data[i].m_key_flags[j])
                continue; // already visited this feature

            // memset(img_marked, 0, num_images * sizeof(bool));
            /* Reset flags */
            int num_touched = touched.size();
            for (int k = 0; k < num_touched; k++)
                img_marked[touched[k]] = false;
            touched.clear();

	    /* Do a breadth first search given this feature */
	    // GetKey(i,j).m_extra = pt_idx;
            m_image_data[i].m_key_flags[j] = true;

	    features.push_back(ImageKey(i, j));
	    features_queue.push(ImageKey(i, j));

            img_marked[i] = true;
            touched.push_back(i);

	    int num_rounds = 0;
	    while (!features_queue.empty()) {
		num_rounds++;

		ImageKey feature = features_queue.front();
		features_queue.pop();
		
		int img1 = feature.first;
		int f1 = feature.second;
                KeypointMatch dummy;
                dummy.m_idx1 = f1;

		int start_idx;
		/* Limit new images to point only to other new images */
		if (img1 >= new_image_start) {
		    start_idx = new_image_start;
		} else {
		    start_idx = 0;
		}

		// for (unsigned int k = start_idx; k < num_images; k++) {
                /* Check all adjacent images */
                // std::list<unsigned int> &nbrs = m_matches.GetNeighbors(img1);
                // int num_nbrs = (int) nbrs.size();
                MatchAdjList &nbrs = m_matches.GetNeighbors(img1);

                // std::list<unsigned int>::iterator iter;
                MatchAdjList::iterator iter;
                for (iter = nbrs.begin(); iter != nbrs.end(); iter++) {
                    // for (int nbr = 0; nbr < num_nbrs; nbr++) {
                    unsigned int k = iter->m_index; // *iter; // nbrs[nbr];

                    if (img_marked[k])
                        continue;

		    /* Skip non-matching images */
		    // if (!ImagesMatch(img1, k))
                    //     continue;

		    MatchIndex base = GetMatchIndex(img1, k);

                    std::vector<KeypointMatch> &list = 
                        m_matches.GetMatchList(base); // m_match_lists[base];

                    /* Do a binary search for the feature */
                    std::pair<std::vector<KeypointMatch>::iterator, 
                              std::vector<KeypointMatch>::iterator> p;

                    p = equal_range(list.begin(), list.end(), 
                                    dummy, MyCompareFirst);

                    if (p.first == p.second)
                        continue;  /* not found */

                    assert((p.first)->m_idx1 == f1);
                    int idx2 = (p.first)->m_idx2;
			    
                    /* Check if we visited this point already */
                    // if (GetKey(k,idx2).m_extra >= 0)
                    //     continue;
                    assert(idx2 < m_image_data[k].GetNumKeys());

                    if (m_image_data[k].m_key_flags[idx2])
                        continue;

                    /* Mark and push the point */
                    // GetKey(k,idx2).m_extra = pt_idx;
                    m_image_data[k].m_key_flags[idx2] = true;
                    features.push_back(ImageKey(k, idx2));
                    features_queue.push(ImageKey(k, idx2));

                    img_marked[k] = true;
                    touched.push_back(k);
		}
	    } /* while loop */

#if 0
	    /* Check for consistency between features */
	    bool consistent;
	    int num_inconsistent = 0;
	    do {
		consistent = true;
		
		for (int k = 0; k < num_images; k++) {
		    int track_size = (int) features.size();
		    int count = 0;
		    for (int l = 0; l < track_size; l++) {
			if (features[l].first == k)
			    count++;
		    }

		    if (count > 1) {
			/* Remove the points associated with this
			 * image */

			for (int l = 0; l < track_size; l++) {
			    if (features[l].first == k) {
				int img = features[l].first;
				int idx = features[l].second;
				
				// GetKey(img,idx).m_extra = LARGE_NUMBER;

				features.erase(features.begin() + l);
				track_size--;
				l--;
			    }
			}

			consistent = false;
			num_inconsistent++;

			break;
		    }
		}
	    } while (!consistent);
#endif
		int degree = (int) features.size();
	    if (degree >= track_degree) {  
		printf("Point with %d projections found\n", 
                       //  (%d inconsistent)\n",
		       (int) features.size()); // , num_inconsistent);
		fflush(stdout);

		tracks.push_back(TrackData(features));

		pt_idx++;
	    } else {
		// printf("Feature only has %d points (%d inconsistent)\n", 
		//       (int) features.size(), num_inconsistent);
	    }

	} /* for loop over features */
    } /* for loop over images */

    printf("[ComputeTracks] Found %d points\n", pt_idx);
    fflush(stdout);

    if (pt_idx != (int) tracks.size()) {
	printf("[ComputeTracks] Error: point count "
	       "inconsistent!\n");
	fflush(stdout);
    }

    /* Clear match lists */
    printf("[ComputeTracks] Clearing match lists...\n");
    fflush(stdout);

    RemoveAllMatches();

    /* Create the new consistent match lists */
    printf("[ComputeTracks] Creating consistent match lists...\n");
    fflush(stdout);

    int num_pts = pt_idx;

    for (int i = 0; i < num_pts; i++) {
	int num_features = (int) tracks[i].m_views.size();

        for (int j = 0; j < num_features; j++) {
            int img1 = tracks[i].m_views[j].first;
            int key1 = tracks[i].m_views[j].second;

            m_image_data[img1].m_visible_points.push_back(i);
            m_image_data[img1].m_visible_keys.push_back(key1);
        }        
    }

    /* Save the tracks */
    m_track_data = tracks;

    // SetMatchesFromTracks();

    printf("[ComputeTracks] Done!\n");
    fflush(stdout);
}

/* Here is the implementation for disambiguation */
void BundlerApp::Disambiguate(float coverage_thres, float score_thres, int new_image_start, int track_degree) 
{
	printf("[----------Disambiguation----------] begins!\n");
	float alpha = 0.0f;   
	int eps = 5; 
    int num_imgs = (int) m_image_data.size();
	int num_tracks = (int) m_track_data.size();
    std::vector<bool> flag_imgs;
    flag_imgs.resize(num_imgs);
    printf("abstracting the scene......\n");
	fflush(stdout);
    for (int i = 0; i < num_imgs; i++)
    {
    	int np = (int) m_image_data[i].m_visible_points.size();
    	flag_imgs[i] = false;
    	printf("image %d sees %d tracks!\n", i, np);
    }
    float delta = -1000000.0f;    // just a very small number
    float pre = 0.0f;
    float coverage = 0.0f;
    std::vector<int> covered_tracks;
    std::vector<int> confusing_tracks;
    std::vector<int> unique_tracks;
    assert((int) covered_tracks.size() == 0 && (int) confusing_tracks.size() == 0);
    do 
    {
    	delta = -1000000.f;
    	std::vector<IntVect> the_tracks;
    	assert((int) the_tracks.size() == 0);
    	int the_img = -1;
    
        for (int i = 0; i < num_imgs; i++)
    	{
    		if (flag_imgs[i])    // for each selected image
    			continue;
    		std::vector<int> &tracks_i = m_image_data[i].m_visible_points;
    		std::vector<IntVect> vs = GetVectorItsectAndNonitsect(covered_tracks, tracks_i);
    		int itsected = (int) vs[0].size();
    		int nonitsected = (int) vs[1].size();
    		float d = float(nonitsected) - alpha * itsected;
    		//float d = float(nonitsected);
    			if (d > delta)
    			{
    				delta = d;
    				the_img = i;
    				the_tracks = vs;
    			}
    		}
    	//if (the_img > -1 && delta > 0.001f)    // delta
    	if (the_img > -1)    // coverage
    	{
    		flag_imgs[the_img] = true;
    		covered_tracks.insert(covered_tracks.end(), the_tracks[1].begin(), the_tracks[1].end());
    		std::vector<IntVect> vs = GetVectorItsectAndNonitsect(confusing_tracks, the_tracks[0]);
    		confusing_tracks.insert(confusing_tracks.end(), vs[1].begin(), vs[1].end());
    		coverage = float((int) covered_tracks.size()) / float(num_tracks);
    		delta = coverage - pre;    // coverage
    		pre = coverage;    // coverage
    		printf("the image %d is selected and current coverage is %f, delta is %f!\n", the_img, coverage, delta);
    	}
    	else
    	{
    		printf(">>>>>>>>>>>>>>>>>>broken-%d(%f)\n", the_img, coverage);
    		break;
    	}
    }
    //while (delta > 0.001f);    // delta
    while (coverage < coverage_thres);    // coverage
    std::vector<IntVect> vect_set = GetVectorItsectAndNonitsect(confusing_tracks, covered_tracks);
    unique_tracks = vect_set[1];
    printf("(%d) the numbers of confusing tracks and unique tracks are %d and %d\n", (int) covered_tracks.size(), (int) confusing_tracks.size(), (int) unique_tracks.size());
    printf("constructing path network......\n");
	fflush(stdout);
    std::vector<IntVect> path_graph;
	path_graph.resize(num_imgs);
	for (int i = 0; i < num_imgs; i++)
	{
		std::vector<int> paths;
		if (flag_imgs[i])    // for each iconic image
    	{
    		//paths = path_graph[i];
    		printf("---iconic image %d connects:", i);
    		//continue;
    	}
    	else
    		printf("---non-iconic image %d connects:", i);
		std::vector<int> &tracks_i = m_image_data[i].m_visible_points;
		int num_nbrs = (int) m_matches.GetNumNeighbors(i);
		if (!num_nbrs)
		{
			printf("\n");
			continue;
		}
		std::vector<int> unique_i = GetVectorIntersection(tracks_i, unique_tracks);
		MatchAdjList &nbrs = m_matches.GetNeighbors(i);
		MatchAdjList::iterator iter;
		for (iter = nbrs.begin(); iter != nbrs.end(); iter++)
		{
			int k = iter->m_index;
			if (!flag_imgs[k])    // for each non-iconic image
    		{
    			//continue;
			}
			std::vector<int> &tracks_k = m_image_data[k].m_visible_points;
			std::vector<int> common_ik = GetVectorIntersection(tracks_i, tracks_k);
			std::vector<int> unique_k = GetVectorIntersection(tracks_k, unique_tracks);
			std::vector<int> unique_ik = GetVectorIntersection(unique_k, unique_i);
			int n = (int) unique_ik.size();
			int m = (int) unique_i.size() > (int) unique_k.size() ? (int) unique_i.size() : (int) unique_k.size();
			if (n > eps)
			{		
				float s = 0.0f;
				s = float(n) / float(m);
				if (s > score_thres)
				{
					paths.push_back(k);
					printf(" %d(%d, %d, %f)", k, n, m, s);
				}
			}
		}
		path_graph[i] = paths;
		printf("\n");
	}
	printf("regenerating tracks......\n");
	fflush(stdout);
	/* Clear former track data */  
	for (int i = 0; i < num_imgs; i++)
	{
		m_image_data[i].m_visible_points.clear();
		m_image_data[i].m_visible_keys.clear();
		m_image_data[i].m_key_flags.clear();
	}
	m_track_data.clear();
	RemoveAllMatches();
	/* Reload and recompute track */
	unsigned long int num_matches_total = 0;
	FILE *f0 = fopen("nmatches.ransac.txt", "r");
	FILE *f1 = fopen("matches.ransac.txt", "r");
	if (f0 == NULL || f1 == NULL) 
	{
		printf("[SifterApp::ReadMatchTableDrew] "
			"Error opening files for reading.\n");
		return;
	}
	int num_images_check = -1;
	fscanf(f0, "%d\n", &num_images_check);
	assert(num_imgs == num_images_check);

	for (int i = 0; i < num_imgs; i++)
	{
		int num_degree = (int) path_graph[i].size();
		for (int j = 0; j < num_imgs; j++) 
		{
			bool isExisted = false;
			for (int t = 0; t < num_degree; t++)
			{
				if (path_graph[i][t] == j)    // is here
				{
					isExisted = true;
					break;
				}
			}
			if (!isExisted)    // not here
			{
				int num_degree1 = (int) path_graph[j].size();
				for (int t = 0; t < num_degree1; t++)
				{
					if (path_graph[j][t] == i)    // is there
					{
						isExisted = true;
						break;
					}
				}
				
			}
			if (!isExisted)
			{
				int num_matches;
				fscanf(f0, "%d", &num_matches);
				if (num_matches > 0) 
				{
					for (int k = 0; k < num_matches; k++) 
					{
						int idx1, idx2;
						fscanf(f1, "%d %d", &(idx1), &(idx2));
					}
				}
				num_matches_total += num_matches;
			}
			else
			{
				MatchIndex idx = GetMatchIndex(i, j);
				int num_matches;
				fscanf(f0, "%d", &num_matches);
				if (num_matches > 0) 
				{
					SetMatch(i, j);
					std::vector<KeypointMatch> &list = m_matches.GetMatchList(idx);
					for (int k = 0; k < num_matches; k++) 
					{
						KeypointMatch m;
						int idx1, idx2;
						fscanf(f1, "%d %d", &(idx1), &(idx2));
#ifdef KEY_LIMIT
						if (idx1 > KEY_LIMIT || idx2 > KEY_LIMIT)
							continue;
#endif /* KEY_LIMIT */
						m.m_idx1 = idx1;
						m.m_idx2 = idx2;
						list.push_back(m);
					}
					num_matches_total += num_matches;
				}
			}
		}
	}
	printf("Read %lu matches in total\n", num_matches_total);
	fclose(f0);
	fclose(f1);
	for (int i = 0; i < num_imgs; i++)
	{
		path_graph[i].clear();
	}
	path_graph.clear();

	MakeMatchListsSymmetric();

	if (num_imgs < 40000)
		WriteMatchTable(".ransac_new");
	ComputeConstraintTracks(new_image_start, track_degree);
	/* Make compatible */
	RemoveAllMatches();

#if 1
	/* Set match flags */
	int num_tracks_new = (int) m_track_data.size();
	for (int i = 0; i < num_tracks_new; i++) {
		TrackData &track = m_track_data[i];
		int num_views = (int) track.m_views.size();

		for (int j = 0; j < num_views; j++) {
			int img1 = track.m_views[j].first;

			assert(img1 >= 0 && img1 < num_imgs);

			for (int k = j+1; k < num_views; k++) {
				int img2 = track.m_views[k].first;

				assert(img2 >= 0 && img2 < num_imgs);

				SetMatch(img1, img2);
				SetMatch(img2, img1);
			}
		}
	}
#endif

	printf("[----------Disambiguation----------] is done!\n");
	fflush(stdout);
}

