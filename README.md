# SfM_Disambiguation
Code for CVPR 2017 paper --- Distinguishing the Indistinguishable: Exploring Structural Ambiguities via Geodesic Context

<h2>About This Work</h2>
<p>This repository contains the code for our Structure-from-Motion Disambiguation work <a href="https://yanqingan.github.io/docs/cvpr17_distinguishing.pdf" rel="nofollow">[1]</a>. It offers a way to distinguish the incorrect feature connections caused by ambiguous image content and recover a unified 3D model.</p>
  
<p>I rewrite the code in order to be contribution-independent and easily-accessible using <a href="https://github.com/snavely/bundler_sfm" rel="nofollow">bundler_sfm</a>, which helps to conduct the incremental Structure-from-Motion calculation. For more robust results, it is recommended to adopt some more recent softwares, like <a href="http://ccwu.me/vsfm/" rel="nofollow">VisualSFM (used in our paper)</a> or <a href="https://github.com/colmap/colmap" rel="nofollow">COLMAP</a>, and accordingly you may need to do a little bit of revision to the interface module. The code is licensed under the GNU General Public License. If you use it for your research, please cite the following paper in your work:</p>
<p>[1] Qingan Yan, Long Yang, Ling Zhang, Chunxia Xiao. <i>Distinguishing the Indistinguishable: Exploring Structural Ambiguities via Geodesic Context</i>. CVPR 2017.</p>

<pre><code>@inproceedings{yan2017distinguishing,
  author={Yan, Qingan and Yang, Long and Zhang, Ling and Xiao, Chunxia},
  title={Distinguishing the indistinguishable: Exploring structural ambiguities via geodesic context},
  booktitle={IEEE Conference on Computer Vision and Pattern Recognition (CVPR)},
  pages={3836-3844},
  year={2017},
  organization={IEEE}
}</code></pre>

<p>For more information, please see my homepage at <a href="https://yanqingan.github.io/" rel="nofollow">https://yanqingan.github.io/</a>. If you have any question, please post it here or send an email to me. Sorry that, I may not respond to you immediately, but I will reach out (suppose I do not forget), if I have spare time beyond R&D work and kissing my little baby.</p>

<h2>File Instruction</h2>
<p>There is currently only one folder ("bundler_sfm_disambiguation" which means it is based on <a href="https://github.com/snavely/bundler_sfm" rel="nofollow">bundler_sfm</a> method) in the directory, in which contains a "src" folder, a "script" folder and a .zip file.</p>
<p><b>Folder "src":</b></p>
<p>1. "Disambiguate.cpp" is the core algorithm implementation file which includes all the steps described in our work [1], where <i>ComputeConstraintTracks()</i> is used to compute long tracks (usually <i>track_degree</i> == 3), and <i>Disambiguate()</i> achieves the disambiguating purpose. They belong to the BundlerApp class.</p>
<p>2. "BundlerApp.h" is the class declaration file where our <i>ComputeConstraintTracks()</i> function (line #201) and <i>Disambiguate()</i> function (line #204) are declared. I only changed these two places as compared to its original version.</p>
<p>3. "BundlerGeometry.cpp" shows where to call our functions (lines from #56 to #61 and #90 to #96); use the new track computing function to form tracks and launch disambiguating after that.</p>
<p>4. "Makefile" indicates how to compile our imported codes; just add a term "Disambiguate.o"(see line #44).</p>
<p><b>Folder "script":</b></p>
<p>"RunBundler_orig.sh" is the original script to run bundler_sfm with full steps of feature selecting, matching and bundler adjustment.</p>
<p>In "RunBundler_skip.sh", I skip the process of feature selecting and matching (see the annotations at line #98 and #109), if you have already finished the whole bundler pipeline once, as it is so expensive to repeat them everytime.</p>

<h2>Parameter Setting</h2>
<p>The parameters, that I think you have to take note of, are the two in <i>Disambiguate()</i> function, i.e., <i>float converage</i> and <i>float score_thres</i>.</p> 
<p>The <i>converage</i> controls how many iconic images will be selected. As for small-scale indoor scenes, a large value between 0.7 and 0.9 is recommended; otherwise, for large-scale unstructured datasets, the value around 0.6 would be enough.</p> 
<p>Parameter <i>score_thres</i> defines whether an image pair is acceptable. As for small-scale scenes, similarly, a large threshold (around 0.3) is recommended; otherwise, for large-scale outdoor scenes, <i>score_thres</i> between 0.04 and 0.1 would be a good choice.</p>
<p>For instance, for <a href="http://www.cs.unc.edu/~jheinly/duplicate_structure.html" rel="nofollow">Alexander Nevsky Cathedral</a> dataset, we use <i>converage</i> = 0.6 and <i>score_thres</i> = 0.1 to achieve well-registered 3D point clouds.</p>

<h2>How to Use</h2>
<p>This code is written in C++ and tested on ubuntu 16.04.</p> 
<ol>
  <li>An easy way to run our code is to directly unzip the "bundler_sfm_disambiguation.zip" file in the document and then run "RunBundler_orig.sh" or "RunBundler_skip.sh" in the directory where images are stored.</li>
  <li>The another way is to first download the bundler_sfm source code at <a href="https://github.com/snavely/bundler_sfm" rel="nofollow">https://github.com/snavely/bundler_sfm</a> then use the files in "src" to replace its original files.</li>  
</ol>
<p>More details about bundler_sfm, please follow the manual at <a href="https://github.com/snavely/bundler_sfm" rel="nofollow">https://github.com/snavely/bundler_sfm</a>.</p>
<p>A simple example:</p>
<pre><code>cd SfM_Disambiguation/
cd bundler_sfm_disambiguation/
unzip bundler_sfm_disambiguation.zip
cd bundler_sfm_disambiguation/
make
cd examples/<b>your_dataset_name/ (cd to the directory where your images are located)</b>
../../Rundler_orig.sh <b>(or ../../Rundler_skip.sh)</b></code></pre>

<h2>Datasets</h2>
<p>With respect to the datasets used in our paper, I have uploaded some of them which are small-scale and difficult to acquire at present. Please also cite these papers (<a href="https://snsinha.github.io/pdfs/RobertsCVPR2011.pdf" rel="nofollow">Richard Roberts-CVPR11</a> and <a href="http://www.cs.sfu.ca/~pingtan/Papers/cvpr12_sfm.pdf" rel="nofollow">Nianjuan Jiang-CVPR12</a>) when you use the datasets in your research.</p>

<p>Because the Temple of Haven dataset is too large, I thus could not upload it into this repository. However, you can still download it through my <a href="https://drive.google.com/file/d/13HFdD7ivEagRXnEEklrrtMIbPKI-2k97/view?usp=sharing" rel="nofollow">GoogleDrive sharing link</a> or <a href="https://pan.baidu.com/s/1JwxtERICu_091EDjKBoAfA" rel="nofollow">BaiduYun sharing link</a> (if you can not access Google because of Firewall). But for my building dataset, I think I have lost it (it is not in my disk); It seems I forgot to backup it when graduating (I suppose I had done it...).</p>

<h2>Other Reminder</h2>
<p>When you use the bundler-sfm toolkit, please do remember to insert your picture EXIF information into the "bundler_sfm/bin/extract_focal.pl" file if the dataset is small-scale and captured from a single camera.</p>
