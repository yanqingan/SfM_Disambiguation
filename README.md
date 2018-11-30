# SfM_Disambiguation
Code for CVPR 2017 paper --- Distinguishing the Indistinguishable: Exploring Structural Ambiguities via Geodesic Context

<h2>About This Work</h2>
<p>This is written by Qingan Yan (<a href="mailto:yanqingan@whu.edu.cn">yanqingan@whu.edu.cn</a> and <a href="mailto:yanqinganssg@gmail.com">yanqinganssg@gmail.com</a>), based on the Structure-from-Motion Disambiguation work [1] of Qingan Yan, Long Yang, Ling Zhang, Chunxia Xiao, at Wuhan Unversity. It offers a way to distinguish the incorrect feature connections caused by ambiguous image content. The code is licensed under the GNU General Public License.</p>

<p>As we do not rewrite the incremental Structure-from-Motion process, the proposed method is thus partly based on "bundler_sfm" [2] (or other similar softwares, like <a href="http://ccwu.me/vsfm/" rel="nofollow">VisualSFM</a> and <a href="https://github.com/colmap/colmap" rel="nofollow">COLMAP</a>). If you use it for your research, please cite the following papers in your work:</p>
<p>[1] Qingan Yan, Long Yang, Ling Zhang, Chunxia Xiao. <i>Distinguishing the Indistinguishable: Exploring Structural Ambiguities via Geodesic Context</i>. CVPR 2017.</p>

<pre><code>@inproceedings{yan2017distinguishing,
  author={Yan, Qingan and Yang, Long and Zhang, Ling and Xiao, Chunxia},
  title={Distinguishing the indistinguishable: Exploring structural ambiguities via geodesic context},
  booktitle={IEEE Conference on Computer Vision and Pattern Recognition (CVPR)},
  pages={3836-3844},
  year={2017},
  organization={IEEE}
}</code></pre>

<p>[2] Noah Snavely, Steven M. Seitz, Richard Szeliski. <i>Photo Tourism: Exploring Photo Collections in 3D</i>. SIGGRAPH 2006.</p>

<pre><code>@article{snavely2006photo,
  title={Photo tourism: exploring photo collections in 3D},
  author={Snavely, Noah and Seitz, Steven M and Szeliski, Richard},
  journal={ACM Transactions on Graphics (TOG)},
  volume={25},
  number={3},
  pages={835--846},
  year={2006},
  publisher={ACM}
}</code></pre>

<p>For more information, please see my homepage at <a href="https://yanqingan.github.io/" rel="nofollow">https://yanqingan.github.io/</a>. If you have any question, please post it here or send an email to me. Sorry that, I may not respond to you immediately, but I will reach out (suppose I do not forget), if I have spare time beyond R&D work and kissing my little baby.</p>

<h2>File Instruction</h2>
<p>There is currently only one folder ("bundler_sfm_disambiguation" which means is based on bundler_sfm method [2]) in the directory, in which contains a "src" folder, a "script" folder and a .zip file.</p>
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
