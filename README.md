# SfM_Disambiguation
Codes of CVPR 2017 paper --- Distinguishing the Indistinguishable: Exploring Structural Ambiguities via Geodesic Context

<h2>About This Work</h2>
<p>This is written by Qingan Yan (<a href="mailto:yanqingan@whu.edu.cn">yanqingan@whu.edu.cn</a> and <a href="mailto:yanqinganssg@gmail.com">yanqinganssg@gmail.com</a>), based on the Structure-from-Motion Disambiguation work [1] of Qingan Yan, Long Yang, Ling Zhang and Chunxia Xiao, at Wuhan Unversity. It offers a way to distinguish the incorrect feature connections caused by ambiguous image content. The code is licensed under the GNU General Public License.</p>

<p>As we do not rewrite the incremental Structure-from-Motion process, the proposed method is thus partly based on "bundler_sfm" [2] (or other similar softwares, like <a href="http://ccwu.me/vsfm/" rel="nofollow">VisualSFM</a> and <a href="https://github.com/colmap/colmap" rel="nofollow">COLMAP</a>). If you use it for your research, please cite the following papers in your work:</p>
<p>[1] Qingan Yan, Long Yang, Ling Zhang and Chunxia Xiao. <i>Distinguishing the Indistinguishable: Exploring Structural Ambiguities via Geodesic Context</i>. CVPR 2017.</p>

<pre><code>@inproceedings{yan2017distinguishing,
  author={Yan, Qingan and Yang, Long and Zhang, Ling and Xiao, Chunxia},
  title={Distinguishing the indistinguishable: Exploring structural ambiguities via geodesic context},
  booktitle={IEEE Conference on Computer Vision and Pattern Recognition (CVPR)},
  pages={3836-3844},
  year={2017},
  organization={IEEE}
}</code></pre>

<p>[2] Noah Snavely, Steven M. Seitz, and Richard Szeliski. <i>Photo Tourism: Exploring Photo Collections in 3D</i>. SIGGRAPH 2006.</p>

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

<p>For more information, see my homepage at <a href="https://yanqingan.github.io/" rel="nofollow">https://yanqingan.github.io/</a>. If you have any question, please post it here or send an email to me. Sorry that, I may not respond to you immediately.</p>

<h2>How to Use</h2>
<p>The code is tested on ubuntu 16.04.</p>

<ol>
<li>An easy way to run our code is to directly unzip the "bundler_sfm_disambiguation.zip" file in the document and then run "RunBundler_orig.sh" or "RunBundler_skip.sh" in the directory where images are stored. More details, please follow the manual of bundler_sfm at <a href="https://github.com/snavely/bundler_sfm" rel="nofollow">https://github.com/snavely/bundler_sfm</a>.</li>
<li>Another way is to first download the bundler_sfm source code at<a href="https://github.com/snavely/bundler_sfm" rel="nofollow">https://github.com/snavely/bundler_sfm</a> then use the files in "src" to replace its original files</li>  
</ol>

<h2>To be continued...</h2>
<p> I will come back again if I have spare time beyond R&D work and kissing my baby.</p>
