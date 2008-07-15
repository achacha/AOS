/* http://keith-wood.name/imageCube.html
   Image Cube for jQuery v1.0.1.
   Written by Keith Wood (kbwood@virginbroadband.com.au) June 2008.
   Dual licensed under the GPL (http://dev.jquery.com/browser/trunk/jquery/GPL-LICENSE.txt) and 
   MIT (http://dev.jquery.com/browser/trunk/jquery/MIT-LICENSE.txt) licenses. 
   Please attribute the author if you use it. */
eval(function(p,a,c,k,e,r){e=function(c){return(c<a?'':e(parseInt(c/a)))+((c=c%a)>35?String.fromCharCode(c+29):c.toString(36))};if(!''.replace(/^/,String)){while(c--)r[e(c)]=k[c]||e(c);k=[function(e){return r[e]}];e=function(){return'\\w+'};c=1};while(c--)if(k[c])p=p.replace(new RegExp('\\b'+e(c)+'\\b','g'),k[c]);return p}('(J($){J 1b(){K.1y={1t:\'1r\',1F:[\'2a\',\'23\',\'L\',\'1j\'],1L:1I,1H:\'34\',1D:1n,1E:\'\',1k:1w,21:1I,X:[0.0,0.8],R:[0.0,1.25],Q:[-0.4,0.0]}};9 A=0;9 B=1;9 C=2;9 D=3;9 E=\'2V\';$.1u(1b.20,{1e:\'2I\',2F:J(a){1B(K.1y,a||{})},2i:J(b,c){b=$(b);O(b.1M(\'.\'+K.1e)){U}b.2r(K.1e);9 d=$.1u({},K.1y,c||{});$.T(b[0],E,d);b.17().1h(J(){9 a=$(K);$.T(K,E,{H:a.G(\'H\'),I:a.G(\'I\'),18:a.G(\'18\'),R:a.G(\'R\'),Q:a.G(\'Q\')});a.G({H:b.G(\'H\'),I:b.G(\'I\'),18:\'1q\',R:d.R[1],Q:d.Q[1]})}).2k(\':2h\').2g();K.1p(b[0])},1p:J(a){a=$(a);9 b=$.T(a[0],E);b.1A=a.17(\':2e\')[0];O(b.1k&&!b.1f){O(!a[0].1C){a[0].1C=\'2Z\'+2c 2Y().2X()}b.1f=2W(\'29.V.1N("#\'+a[0].1C+\'")\',b.21)}$.T(a[0],E,b)},1N:J(a,b){a=$(a);K.1z(a[0],1w);9 c=$.T(a[0],E);9 d={};d[E]=1.0;a.2S(E,0.0).2O(d,c.1L,c.1H,b)},2N:J(a){U $.T(a,E).1A},1z:J(a,b){9 c=$.T(a,E);O(c.1f){2M(c.1f);c.1f=1l}O(!b){c.1k=1n}$.T(a,E,c)},2K:J(a){K.1X(a,{1k:1w})},1X:J(a,b){9 c=$.T(a,E);1B(c||{},b||{});$.T(a,E,c);K.1p(a)},2G:J(a){K.1z(a);a=$(a);O(!a.1M(\'.\'+K.1e)){U}a.2E().2C(K.1e).17(\'.1v\').1G();a.17().1h(J(){$(K).G($.T(K,E)).28();$.27(K,E)});$.27(a[0],E)},2b:J(d){9 e=$.T(d,E);9 d=$(d);9 f=1n;9 g=d.17(\':2e\');9 h=g.2u();h=(h.1c?h:d.17(\':2h\'));9 j=d.1s();d.2t().1h(J(){9 a=$(K);O(a.G(\'18\')==\'2s\'){j.L-=a.1s().L;j.N-=a.1s().N;U 1n}});9 k={H:d.H(),I:d.I()};9 l=(e.1t!=\'1r\'?e.1t:e.1F[1i.2q(1i.1r()*e.1F.1c)]);l=1i.1K(0,$.1J(l,[\'2a\',\'23\',\'L\',\'1j\']));9 m=(l==A||l==B);9 n=(l==C||l==D);9 o=(l==A||l==C);9 q=(o?e.X[0]:e.X[1]);9 r=[];9 s=J(p){9 b=[];15(9 i=0;i<4;i++){b[i]=p.G(\'2p\'+[\'2o\',\'2n\',\'2m\',\'2l\'][i]+\'2j\');9 a=W(b[i]);b[i]=(!33(a)?a:1i.1K(0,$.1J(b[i],[\'32\',\'31\',\'30\'])*2+1))}U b};r[0]=s(g);r[1]=s(h);9 t=[];t[0]=[W(g.G(\'P-L\')),W(g.G(\'P-1j\')),W(g.G(\'P-N\')),W(g.G(\'P-2f\'))];t[1]=[W(h.G(\'P-L\')),W(h.G(\'P-1j\')),W(h.G(\'P-N\')),W(h.G(\'P-2f\'))];9 u=[];u[0]=[($.1o?r[0][0]+r[0][1]+t[0][0]+t[0][1]:0),($.1o?r[0][2]+r[0][3]+t[0][2]+t[0][3]:0)];u[1]=[($.1o?r[1][0]+r[1][1]+t[1][0]+t[1][1]:0),($.1o?r[1][2]+r[1][3]+t[1][2]+t[1][3]:0)];9 v=[];v[0]={M:g,L:{5:j.L,7:j.L+(l==D?k.H:0),F:\'6\'},H:{5:k.H-u[0][0],7:(m?k.H-u[0][0]:0),F:\'6\'},N:{5:j.N,7:j.N+(l==B?k.I:0),F:\'6\'},I:{5:k.I-u[0][1],7:(m?0:k.I-u[0][1]),F:\'6\'},1a:{5:t[0][0],7:(n?0:t[0][0]),F:\'6\'},14:{5:t[0][1],7:(n?0:t[0][1]),F:\'6\'},13:{5:t[0][2],7:(m?0:t[0][2]),F:\'6\'},19:{5:t[0][3],7:(m?0:t[0][3]),F:\'6\'},Z:{5:r[0][0],7:(n?0:r[0][0]),F:\'6\'},12:{5:r[0][1],7:(n?0:r[0][1]),F:\'6\'},Y:{5:r[0][2],7:(m?0:r[0][2]),F:\'6\'},11:{5:r[0][3],7:(m?0:r[0][3]),F:\'6\'},R:{5:e.R[1],7:(m?e.R[0]:e.R[1]),F:\'16\'},Q:{5:e.Q[1],7:(m?e.Q[1]:e.Q[0]),F:\'16\'}};v[1]={M:h,L:{5:j.L+(l==C?k.H:0),7:j.L,F:\'6\'},H:{5:(m?k.H-u[1][0]:0),7:k.H-u[1][0],F:\'6\'},N:{5:j.N+(l==A?k.I:0),7:j.N,F:\'6\'},I:{5:(m?($.1d.1g?1:0):k.I-u[1][1]),7:k.I-u[1][1],F:\'6\'},1a:{5:(n?0:t[1][0]),7:t[1][0],F:\'6\'},14:{5:(n?0:t[1][1]),7:t[1][1],F:\'6\'},13:{5:(m?0:t[1][2]),7:t[1][2],F:\'6\'},19:{5:(m?0:t[1][3]),7:t[1][3],F:\'6\'},Z:{5:(n?0:r[1][0]),7:r[1][0],F:\'6\'},12:{5:(n?0:r[1][1]),7:r[1][1],F:\'6\'},Y:{5:(m?0:r[1][2]),7:r[1][2],F:\'6\'},11:{5:(m?0:r[1][3]),7:r[1][3],F:\'6\'},R:{5:(m?e.R[0]:e.R[1]),7:e.R[1],F:\'16\'},Q:{5:(m?e.Q[1]:e.Q[0]),7:e.Q[1],F:\'16\'}};O(!e.1D){9 w=J(a,b,c){U{M:1l,L:{5:a.L.5,7:a.L.7,F:\'6\'},H:{5:a.H.5,7:a.H.7,F:\'6\'},N:{5:a.N.5,7:a.N.7,F:\'6\'},I:{5:a.I.5,7:a.I.7,F:\'6\'},1a:{5:a.1a.5+a.Z.5,7:a.1a.7+a.Z.7,F:\'6\'},14:{5:a.14.5+a.12.5,7:a.14.7+a.12.7,F:\'6\'},13:{5:a.13.5+a.Y.5,7:a.13.7+a.Y.7,F:\'6\'},19:{5:a.19.5+a.11.5,7:a.19.7+a.11.7,F:\'6\'},X:{5:b,7:c,F:\'\'}}};v[2]=w(v[o?0:1],q,e.X[1]-q);v[3]=w(v[o?1:0],e.X[1]-q,q)}9 x=J(a){U{L:a.L.5+\'6\',H:a.H.5+\'6\',N:a.N.5+\'6\',I:a.I.5+\'6\',R:a.R.5+\'16\',P:a.13.5+\'6 \'+a.14.5+\'6 \'+a.19.5+\'6 \'+a.1a.5+\'6\',Z:a.Z.5+\'6\',12:a.12.5+\'6\',Y:a.Y.5+\'6\',11:a.11.5+\'6\',Q:a.Q.5+\'16\',2U:\'2T\'}};g.G(x(v[0]));h.G(x(v[1])).28();O(!e.1D){v[2].M=$(($.1d.1g?\'<26 1V="\'+e.1E+\'2R.24"\':\'<1m\')+\' 22="1v" 1Q="1U-1Z: 2L; X: \'+q+\'; z-1Y: 10; 18: 1q; \'+\'L: \'+v[2].L.5+\'6; H: \'+v[2].H.5+\'6; \'+\'N: \'+v[2].N.5+\'6; I: \'+v[2].I.5+\'6; \'+\'P: \'+(o?g:h).G(\'P\')+\';"\'+($.1d.1g?\'/>\':\'></1m>\'));v[3].M=$(($.1d.1g?\'<26 1V="\'+e.1E+\'2J.24"\':\'<1m\')+\' 22="1v" 1Q="1U-1Z: 2H; X: \'+(e.X[1]-q)+\'; z-1Y: 10; 18: 1q; \'+\'L: \'+v[3].L.5+\'6; H: \'+v[3].H.5+\'6; \'+\'N: \'+v[3].N.5+\'6; I: \'+v[3].I.5+\'6; \'+\'P: \'+(o?h:g).G(\'P\')+\';"\'+($.1d.1g?\'/>\':\'></1m>\'));d.1W(v[2].M).1W(v[3].M)}15(9 i=0;i<v.1c;i++){15(9 y 1x v[i]){9 z=v[i][y];z.1T=z.7-z.5}}U v}});J 1B(a,b){$.1u(a,b);15(9 c 1x b){O(b[c]==1l){a[c]=1l}}U a}$.2D.V=J(a){9 b=2P.20.2Q.2B(2A,1);O(a==\'1A\'){U $.V[\'1S\'+a+\'1b\'].2d($.V,[K[0]].1R(b))}U K.1h(J(){O(2z a==\'2y\'){$.V[\'1S\'+a+\'1b\'].2d($.V,[K].1R(b))}2x{$.V.2i(K,a)}})};$.2w.2v[E]=J(a){O(a.1P==0){a.5=0.0;a.7=1.0;a.S=$.V.2b(a.M);a.1O={Z:a.S[0].M.G(\'Z\'),12:a.S[0].M.G(\'12\'),Y:a.S[0].M.G(\'Y\'),11:a.S[0].M.G(\'11\'),P:a.S[0].M.G(\'P\')}}15(9 i=0;i<a.S.1c;i++){9 b={};15(9 c 1x a.S[i]){9 d=a.S[i][c];O(c!=\'M\'){b[c]=(a.35*d.1T+d.5)+d.F}}a.S[i].M.G(b)}O(a.1P==1){a.S[0].M.2g().G(a.1O);O(a.S.1c>2){a.S[2].M.1G();a.S[3].M.1G()}$.V.1p(a.M)}};$.V=2c 1b()})(29);',62,192,'|||||start|px|end||var||||||||||||||||||||||||||||||||units|css|width|height|function|this|left|elem|top|if|padding|letterSpacing|lineHeight|stepProps|data|return|imagecube|parseFloat|opacity|borderTopWidth|borderLeftWidth||borderBottomWidth|borderRightWidth|paddingTop|paddingRight|for|em|children|position|paddingBottom|paddingLeft|ImageCube|length|browser|markerClassName|_timer|msie|each|Math|right|repeat|null|div|false|boxModel|_prepareRotation|absolute|random|offset|direction|extend|imageCubeShading|true|in|_defaults|_stopImageCube|current|extendRemove|id|noShading|imagePath|randomSelection|remove|easing|2000|inArray|max|speed|is|_rotateImageCube|saveCSS|state|style|concat|_|diff|background|src|append|_changeImageCube|index|color|prototype|pause|class|down|png||img|removeData|show|jQuery|up|_prepareImageCube|new|apply|visible|bottom|hide|first|_attachImageCube|Width|not|Bottom|Top|Right|Left|border|floor|addClass|fixed|parents|next|step|fx|else|string|typeof|arguments|call|removeClass|fn|stop|setDefaults|_destroyImageCube|black|hasImageCube|imageCubeShad|_startImageCube|white|clearTimeout|_currentImageCube|animate|Array|slice|imageCubeHigh|attr|hidden|overflow|imageCube|setTimeout|getTime|Date|ic|thick|medium|thin|isNaN|linear|pos'.split('|'),0,{}))