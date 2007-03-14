Rich Text Wysiwyg Editor Class
==============================
made by Jacob Lee


User's short Manual
===================

Analyze the demo.htm and custom.htm for your use.
Usual use of RichText class is as following:

at least the following code should be in the head or body of the document.
[code]
	<script type="text/javascript" src="lang/en.js"></script>
	<script type="text/javascript" src="richtext.js"></script>
[/code]

if you want to use generateXHTML add this code.
[code]
	<script type="text/javascript" src="html2xhtml.js"></script>
[/code]

The format of use :
[code]
//Usage: var object_name = new RichText('object_name', html, width, height, buttons, readOnly)
var rte1 = new RichText('rte1', 'here&#39;s the "<em>preloaded</em> <b>content</b>"', 400, 200, true, false);
rte1.setToolbarItems(['Bold','Italic','Underline','|','JustifyLeft','JustifyCenter','JustifyRight','JustifyFull','|','InsertHorizontalRule','blanktd']);
rte1.create();
[/code]

the variable name and the first parameter's string value should be the same.

You can change the format like
[code]
var rte1 = new RichText('rte1');
rte1.value = 'here&#39;s the "<em>preloaded</em> <b>content</b>"';
rte1.width = 400;
rte1.height = 200;
rte1.showButtons = true;
rte1.readOnly = false;
rte1.setToolbarItems(['Bold','Italic','Underline','|','JustifyLeft','JustifyCenter','JustifyRight','JustifyFull','|','InsertHorizontalRule','blanktd']);
rte1.create();
[/code]

*** Experimental ***
When the width is greater( or same) than 800px the side menu will be automatically made.
This will be removed or improved according to the users' future suggestions.
*** Experimental ***

If you want to use the custom toolbar, read carefully.
Your toolbar folder should be inside the 'style' folder, the folder name will be the style name.
By default inside the style folder there is 'default' folder which has images folder for icons and rte.css file.

when you use your custom toolbar, first put everything at right place. second, make a Toolbar object like:
[code]
var tb = new Toolbar();

tb.style = 'default';
tb.items = ['Bold','Italic','Underline','|','ForeColor','HiliteColor','|','JustifyLeft','JustifyCenter','JustifyRight','JustifyFull','|','InsertHorizontalRule','blanktd'];
tb.show = true;
tb.imgPath = 'images/';
tb.cssFile = 'rte.css';
[/code]

the value of tb.style in the example is the style folder name. if you put your css file as 'richtext.css' and icons inside the 'mystyle' folder and the icons is in the 'images' folder inside the 'mystyle' foloder then the setting will be
[code]
var tb = new Toolbar();
tb.style = 'mystyle';
tb.cssFile = 'richtext.css';
[/code]

The value of the property 'items' is an array of commands. the icon file name should be the same of the command. Look inside the default folder and you will understand what I'm saying.

After you make your own Toolbar object, make it the parameter of 'create' function.
[code]
var tb = new Toolbar();

tb.style = 'myToolbar';
tb.items = ['Bold','Italic','Underline','|','ForeColor','HiliteColor','|','JustifyLeft','JustifyCenter','JustifyRight','JustifyFull','|','InsertHorizontalRule','blanktd'];
tb.show = true; // show toolbar.
tb.imgPath = 'myImg/';
tb.cssFile = 'my.css';

var rte = new RichText('rte', '', 300, 300);
rte.create(tb); // apply our own toolbar to rte
[/code]

Where to download the source file?
==================================

http://jacobswell.nared.net/rte/rte.zip

Can I see Demo?
===============

http://jacobswell.nared.net/rte/demo.htm
http://jacobswell.nared.net/rte/multi.htm
http://jacobswell.nared.net/rte/custom.htm

