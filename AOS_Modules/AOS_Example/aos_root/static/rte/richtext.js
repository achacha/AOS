/*
** Cross-browser RichText Editor. Version: 1.0.01
** 
** Inspired by Kevin Roth's RichText Editor and Revamped version of Timothy Bell
** This file is on the PUBLIC DOMAIN.
** Jacob Lee (letsgolee@lycos.co.kr)
*/

/*
** - Change version number to 1.0.01. 09 Feb, 2006.
** - Edit the size of popup window of pasteFromWord and pastePlainText in Firefox. 09 Feb, 2006.
** - Confirm Cut, Copy, Paste only in IE. 09 Feb, 2006.
** - make "Enalbe Change Mode" option. when enableChangeMode is false, the mode changer will not be displayed. 09 Feb, 2006.
** - Making internal manual. 07 Feb, 2006.
** - Starting version 1.0 -- 04 Feb, 2006.
*/

/* Set the navigator userAgent for detecting user's browser */
function UA() {
	var t = this;
	t.ua = navigator.userAgent.toLowerCase();
	t.opera = t.ua.indexOf('opera') != -1;
	t.ie = t.ua.indexOf('msie') != -1 && !t.opera && t.ua.indexOf('webtv') == -1;
	t.gecko = t.ua.indexOf('gecko') != -1;
	t.safari = t.ua.indexOf('safari') != -1;
	t.konqueror = t.ua.indexOf('konqueror') != -1;
	t.compatible = document.getElementById && document.designMode;
};

var ua = new UA();

/* the language and encoding for generateXHTML */
var language = "en";
var encoding = "utf-8";

/* Base path of the script.
**
** It is better when you register this script in any document and write down the whole path,
** especially when we think the image path.
*/
var basePath = '';
var s = document.getElementsByTagName("SCRIPT");
for(var i = 0; i < s.length; i++) {
	if(s[i].src.toLowerCase().indexOf("richtext.js") != -1) {
		basePath = s[i].src.replace(/richtext.js/, '');
		break;
	}
}

var allRTEs = [];
var dialog;
var dialogArgs;

/* Toolbar definition
**
** You can make your own toolbar using this definition.
** style - the style folder name in the folder named 'style'
** items - your icons array. if empty defaultItems will be used.
** show - if true the toolbar will be shown.
** imgPath - if the icons are in the different folder change the value.
** iconHeight - if the height of icons are different change it.
** cssFile - if you use other named css file change it.
**
** I suggest you don't change other properties.
**
** Ex)
**   var tb = new Toolbar();
**   tb.style = 'mystyle';
**   tb.items = ['Bold','Italic','Underline'];
**   tb.cssFile = 'mycss.css';
**
**   var rte = new RichText('rte');
**   rte.width = '700';
**   rte.height = 300;
**   rte.create(tb);
*/
function Toolbar() {
	var t = this;
	t.style = 'default';
	t.items;
	t.defaultItems = ['Save','Preview','Print','Search','SelectAll','WordCount','SpellCheck','|',
		/* 'Cut','Copy','Paste','|', */'PastePlainText','PasteFromWord','|',/* 'PasteAsHtml', */'Undo','Redo','|',
		'InsertOrderedList','InsertUnorderedList','|',
		'Outdent','Indent','|','ForeColor','HiliteColor','|',
		'HyperLink','InsertImage'/*,'Smilies'*/,'InsertTable','|',
		'SpecialChar','InsertHorizontalRule','RemoveFormat',
		'blanktd','newline',
		'FormatBlock','FontName','FontSize','|',
		'Bold','Italic','Underline','|',
		'StrikeThrough','Superscript','Subscript','|',
		'JustifyLeft','JustifyCenter','JustifyRight','JustifyFull',
		'blanktd'];
	t.itemObj = {};
	t.lineBg = 'bg.gif';
	t.line = 1;
	t.show = true;
	t.path;
	t.imgPath = 'images/';
	t.cssFile = 'rte.css';
	t.lineHeight = 27;
	t.iconHeight = 24;
};

/*
** RichText Class definition
**
** id - should have the same value with object name. if the object name is 'rte' the id should 'rte'.
**      this id is used internally. and the real use of this value is textarea id.
**      so this id will be the id and name of the textarea. when a form is submitted, you can access the textarea
**      using this id and name. ie) document.theForm.rte.value
** html - initial html code value.
** width - the width of wysiwyg editor.
** height - the height of wysiwyg editor.
** showButtons - if true the toolbar will be shown.
** readOnly - if true the value will be read only. you cannot edit it.
** generateXHTML - if true it will generate XHTML. you have to have XHTML gereratiing script file installed.
** keepAbsolute - if true absolute path will be used for link and image in IE.
**                By default IE will try to convery all hyperlinks to absolute paths.
**                By setting this value to "false" it will retain the relative path.
**
** Ex)
**   new rte = RichText('rte', 'initial html value', 300, 300, true, false);
**   rte.create();
** or
**   new rte = RichText('rte');
**   rte.value = 'initial html value';
**   rte.width = 300;
**   rte.height = 300;
**   rte.showButtons = true;
**   rte.readOnly = false;
**   rte.generateHTML = false;
**   rte.keepAbsolute = true;
**   rte.enableChangeMode = false;  // disable source mode
**   rte.create();
*/
function RichText(id, html, width, height, showButtons, readOnly) {
	var t = this;
	t.rteID = id;
	t.tb = new Toolbar();
	t.readOnly = typeof readOnly != 'undefined' ? readOnly : false;
	t.showButtons = typeof showButtons != 'undefined' ? showButtons : true;
	t.mode = 'design';
	t.minimalSize = 800;
	t.sideMenu = false;
	t.sideMenuSize = 200;

	t.width = typeof width != 'undefined' ? width : 540;
	t.height = typeof height != 'undefined' ? height : 300;
	t.cssFile;
	t.generateXHTML = false;
	t.value = typeof html != 'undefined' ? html : '';
	t.keepAbsolute = true;
	t.enableChangeMode = true;

	/* functions */
	t.create = create;
	t.focus = focus;
	t.execCommand = execCommand;
	t.enableDesignMode = enableDesignMode;
	t.updateTextarea = updateTextarea;
	t.toggleHTMLSrc = toggleHTMLSrc;
	t.getRange = getRange;
	t.geckoKeyPress = geckoKeyPress;
	t.ieKeyPress = ieKeyPress;
	t.selectFont = selectFont;
	t.modifyOnSubmit = modifyOnSubmit;
	t.checkSpell = checkSpell;
	t.dlgColorPalette = dlgColorPalette;
	t.insertHTML = insertHTML;
	t.updateToolbarState = updateToolbarState;
	t.hideDropMenu = hideDropMenu;
	t.setCSSFile = setCSSFile;
	t.setToolbarStyle = setToolbarStyle;
	t.setToolbarItems = setToolbarItems;
	t.changeMode = changeMode;
	t.toggleSideMenu = toggleSideMenu;
	t.showGuideLines = showGuideLines;
	t.hideGuideLines = hideGuideLines;
	t.getWindow = getWindow;
	t.getTextarea = getTextarea;
	t.getPalette = getPalette;
	t.popUpWin = popUpWin;
	t.drawSideMenu = drawSideMenu;
	t.countWords = countWords;
};

function setCSSFile(css) {
	this.cssFile = css;
};

/* register toolbar items. it does not change the icon style */
function setToolbarStyle(stl) {
	this.tb.style = stl;
};

function getWindow() {
	return document.getElementById(this.rteID + '_iframe').contentWindow;
};

function getTextarea() {
	return document.getElementById(this.rteID);
};

function getPalette() {
	return document.getElementById(this.rteID + '_palette');
};

function toggleSideMenu() {
	var t = this;
	var iframe = document.getElementById(t.rteID + '_iframe');
	var sideMenu = document.getElementById(t.rteID + '_sideMenuDiv');
	
	if (sideMenu.style.display == 'none') {
		var width = getInt(t.width) - t.sideMenuSize;
		width = (ua.ie) ? width + 'px' : (width - 4) + 'px';
		iframe.style.width = width;
		sideMenu.style.display = 'block';
	} else { // display == 'block'
		var width = getInt(t.width);
		width = (ua.ie) ? width + 'px' : (width - 4) + 'px';
		iframe.style.width = width;
		sideMenu.style.display = 'none';
	}
};

function setToolbarItems(items) {
	if (typeof items == 'object') {
		this.tb.items = items;
	}
};

function getInt(v) {
	if (typeof v == 'string' && v.toLowerCase().indexOf('px') != -1) {
		return parseInt(v.replace('px',''));
	}
	return parseInt(v);
};

function create(toolbar) {
	var t = this;
	var d = document;
	t.width = getInt(t.width);
	t.height = getInt(t.height);

	if (!ua.compatible) {
		d.writeln('div style="color:gray">' + lang['NotCompatible'] + '</div><br />');
		d.writeln('<input type="radio" name="' + t.rteID + '_autobr" value="1" checked="checked" onclick="autoBRon(\' ' + t.rteID + '\')" />' + lang['AutoBR'] + '<input type="radio" name="' + t.rteID + '_autobr" value="0" onclick="autoBRoff(\' ' + t.rteID + '\')" />' + lang['RawHTML'] + '<br />');

		d.writeln('<textarea name="' + t.rteID + '" id="' + t.rteID + '" style="width:' + t.width + 'px;height:' + t.height + 'px;"' + (t.readOnly ? 'readonly' : '') + '>' + t.value + '</textarea>');
		return;
	} else {
		if (typeof toolbar == 'object') {
			t.tb = toolbar;
		} else {
			t.tb.show = t.showButtons;
		}
		t.tb.path = basePath + 'style/' + t.tb.style + '/';
		t.tb.imgPath = t.tb.path + t.tb.imgPath;
		t.tb.cssFile = t.tb.path + t.tb.cssFile;

		/* document.writeln('<style type="text/css">@import "' + t.tb.cssFile + '";</style>');
		   style tag registering in IE is not possible. instead we register link tag. */
		var head = d.getElementsByTagName('HEAD')[0];
		var linkArr = d.getElementsByTagName('LINK');
		var registered = false;
		for (var i in linkArr) {
			if (linkArr[i].href == t.tb.cssFile) {
				registered = true;
				break;
			}
		}
		if (!registered) {
			var link = d.createElement('LINK');
			link.rel = 'stylesheet';
			link.type = 'text/css';
			link.href = t.tb.cssFile;
			head.appendChild(link);
		}
	}
	
	if (t.readOnly) t.tb.show = false;
	t.width = t.width < 540 ? (ua.ie || ua.opera ? 540 : 544) : (ua.ie || ua.opera ? t.width : t.width + 4);

	if (!t.readOnly) {
		d.writeln('<div class="rteDiv" style="overflow:hidden;width:' + t.width + 'px">');
	}
	if (t.tb.show) {
		t.tb.items = t.tb.items || t.tb.defaultItems;

		function tbLineStart(lineNo) {
			return '<table cellpadding=0 cellspacing=0 border=0 id="' + t.rteID + '_tbLine_' + lineNo + '" width="' + t.width + '">' +
				'<tr class="rteBk" ' + (t.tb.lineBg && t.tb.lineBg.length ? 'style="margin:2px;background-image:url(\'' + t.tb.imgPath + t.tb.lineBg + '\')"' : 'style="margin:2px;background:#DEDCD5"') + '><td>&nbsp;</td><td><img src="' + t.tb.imgPath + 'dots.gif"></td><td>&nbsp;</td>';
		}
		function tbLineEnd() {
			return '</tr></table>';
		}

		d.writeln(tbLineStart(t.tb.line));

		if (t.width >= t.minimalSize) {
			// Add side menu button
			var b = 'SideMenu';
			d.writeln('<td><img id="' + t.rteID + '_' + b + '" class="rteImage" src="' + t.tb.imgPath + b + '.gif" alt="' + lang[b] + '" title="' + lang[b] + '"></td>');
			var img = d.getElementById(t.rteID + '_' + b);
			img.onmouseover = function() {this.className = 'rteImageUp';};
			img.onmouseout  = function(evt) {this.className = 'rteImage'; t.updateToolbarState(evt, true);};
			img.onclick = function() {this.className = 'rteImageDown'; t.toggleSideMenu();};
			t.tb.itemObj[b] = img;
		}

		for (m in t.tb.items) {
			var b = t.tb.items[m];

			switch(b) {
				case 'FormatBlock':
				case 'FontName':
				case 'FontSize':
					d.writeln('<td  class="rteSelectBk">');
					d.writeln('<select id="' + t.rteID + '_' + b + '" class="rteSelect">');
					d.writeln(lang[b]);
					d.writeln('</select>');
					d.writeln('</td>');
					var obj = d.getElementById(t.rteID + '_' + b);
					obj.cmd = b;
					obj.onchange = function() {t.selectFont(this.cmd, this.value);};
					t.tb.itemObj[b] = obj;
					break;
				case 'blanktd':
					d.writeln('<td width="100%"></td>');
					break;
				case 'newline':
					t.tb.line++;
					d.writeln(tbLineEnd());
					d.writeln(tbLineStart(t.tb.line));
					break;
				case '|':
					d.writeln('<td><img class="rteSep" src="' + t.tb.imgPath + 'Separator.gif"></td>');
					break;
				default:
					if ((b == 'SpellCheck' || b == 'Cut' || b == 'Copy' || b == 'Paste') && !ua.ie) break;
					d.writeln('<td><img id="' + t.rteID + '_' + b + '" class="rteImage" src="' + t.tb.imgPath + b + '.gif" alt="' + lang[b] + '" title="' + lang[b] + '"></td>');
					var img = d.getElementById(t.rteID + '_' + b);
					img.cmd = b;
					img.onmouseover = function() {this.className = 'rteImageUp';};
					img.onmouseout  = function(evt) {this.className = 'rteImage'; t.updateToolbarState(evt, true);};
					img.onclick = function() {this.className = 'rteImageDown'; t.execCommand(this.cmd, '');};
					t.tb.itemObj[b] = img;
					break;
			}
		}

		/* register all other buttons as hidden input.
		** we will not append it to the form. just make it for updateToolbarState function.
		** these inputs will not be submitted.
		*/
		for (var i in t.tb.defaultItems) {
			var b = t.tb.defaultItems[i];
			if (b == '|' || b == 'newline' || b == 'blanktd') continue;
			if (!t.tb.itemObj[b]) {
				var inp = d.createElement('INPUT');
				inp.type = 'hidden';
				t.tb.itemObj[b] = inp;
			}
		}
		d.writeln(tbLineEnd());
	}

	if (!t.readOnly) {
		d.writeln('<table cellspacing=0 cellpadding=0 border=0><tr><td width="100%"><iframe id="' + t.rteID + '_iframe' + '" name="' + t.rteID + '_iframe' + '" style="width:' + (ua.ie ? t.width : t.width - 4) + 'px;height:' + t.height + 'px" src="' + basePath + 'blank.htm"></iframe></td>');

		if (t.width >= t.minimalSize) {
			// Show side menu
			t.drawSideMenu();
		}
		d.writeln('</tr></table>');

		if (t.enableChangeMode) {		
			d.writeln('<table cellpadding=0 cellspacing=0 border=0 width="' + t.width + '"><tr class="rteModeBk"><td height="27px"><table cellpadding=0 cellspacing=0 border=0 width="100%"><tr><td height="25px">&nbsp;</td>' + 
				// design mode
				'<td class="rteMode" id="' + t.rteID + '_designMode" nowrap><table cellpadding=0 cellspacing=0 border=0><tr><td><img src="' + t.tb.imgPath + 'DesignMode.gif"></td><td class="rteModeText" nowrap>&nbsp;' + lang['DesignMode'] + '&nbsp;</td></tr></table></td>' + 
				// source mode
				'<td class="rteMode" id="' + t.rteID + '_sourceMode" nowrap><table cellpadding=0 cellspacing=0 border=0><tr><td><img src="' + t.tb.imgPath + 'SourceMode.gif"></td><td class="rteModeText" nowrap>&nbsp;' + lang['SourceMode'] + '&nbsp;</td></tr></table></td>' + 
				'<td width="100%" align="right" class="rteText" nowrap>' + lang['RichTextInfo'] + '&nbsp;&nbsp;</td></tr></table></td></tr></table>');

			d.getElementById(t.rteID + '_designMode').onclick = function() {t.changeMode('design');};
			d.getElementById(t.rteID + '_sourceMode').onclick = function() {t.changeMode('source');};
		}

		if (t.tb.show) {
			if (ua.ie) {
				var paletteWidth = 172;
				var paletteHeight = 121;
			} else {
				var paletteWidth = 176;
				var paletteHeight = 125;
			}
			d.writeln('<iframe width="' + paletteWidth + '" height="' + paletteHeight + '" id="' + t.rteID + '_palette' + '" src="' + basePath + 'palette.htm" marginwidth="0" marginheight="0" scrolling="no" frameBorder=0 style="visibility:hidden;position:absolute;"></iframe>');
		}
	}
	d.writeln('<textarea name="' + t.rteID + '" id="' + t.rteID + '" style="width:' + t.width + ';height:' + t.height + (!t.readOnly ? ';display:none' : '') + ';"' + (t.readOnly ? 'readonly' : '') + '>' + t.value + '</textarea>');
	if (!t.readOnly) {
		d.writeln('</div>');
		t.enableDesignMode();
		t.modifyOnSubmit();
		allRTEs.push(t);
	}
};

function drawSideMenu() {
	var t = this;
	t.sideMenu = true;
	document.writeln('<td style="vertical-align:top"><div id="' + t.rteID + '_sideMenuDiv" style="width:' + (t.sideMenuSize - 5) + 'px;display:none"><div class="rteDiv" style="width:100%"><table cellpadding=0 cellspacing=0 border=0 width="100%"><tr class="rteBk"' + (t.tb.lineBg && t.tb.lineBg.length ? 'style="text-align:center;margin:2px;background-image:url(\'' + t.tb.imgPath + t.tb.lineBg + '\')"' : 'style="margin:2px;background:#DEDCD5"') + '><td align="center"><span class="rteText"><b>' + lang['HyperLink'] + '</b></span></td></tr>' +
	'<tr><td style="padding:3;text-align:center" class="rteModeText"><input id="' + t.rteID + '_linkInput" class="rteInput" type="text" size="40"><div style="text-align: center;margin:5"><button type="button" id="' + t.rteID + '_linkBtn" class="rteBtn">' + lang['HyperLink'] + '</button></div></td></tr>' +
	'<tr class="rteBk"' + (t.tb.lineBg && t.tb.lineBg.length ? 'style="text-align:center;margin:2px;background-image:url(\'' + t.tb.imgPath + t.tb.lineBg + '\')"' : 'style="margin:2px;background:#DEDCD5"') + '><td align="center"><span class="rteText"><b>' + lang['InsertImage'] + '</b></span></td></tr>' +
	'<tr><td style="padding:3;text-align:center" class="rteModeText"><input id="' + t.rteID + '_imageInput" class="rteInput" type="text" size="40"><div style="text-align: center;margin:5"><button type="button" id="' + t.rteID + '_imageBtn" class="rteBtn">' + lang['InsertImage'] + '</button></div></td></tr>' +
	'<tr class="rteBk"' + (t.tb.lineBg && t.tb.lineBg.length ? 'style="text-align:center;margin:2px;background-image:url(\'' + t.tb.imgPath + t.tb.lineBg + '\')"' : 'style="margin:2px;background:#DEDCD5"') + '><td align="center"><span class="rteText"><b>' + lang['InsertTable'] + '</b></span></td></tr>' +
	'<tr><td style="padding:3;text-align:center" class="rteModeText"><table cellpadding="0" cellspacing="0" border="0" width=100%><tr><td class="rteModeText" align="right">' + lang['TableRows'] + ':&nbsp;</td><td><input class="rteInput" type="text" id="' + t.rteID + '_rowsInput" value="2" size="4"></td><td class="rteModeText" align="right" valign=middle nowrap>&nbsp;' + lang['TableColumns'] + ':&nbsp;</td><td><input class="rteInput" type="text" id="' + t.rteID + '_colsInput" value="2" size="4"></td></tr><tr><td class="rteModeText" align="right">' + lang['TableWidthAbbr'] + ':&nbsp;</td><td><input class="rteInput" name="width" type="text" id="' + t.rteID + '_widthInput" value="100" size="4"></td><td class="rteModeText" align="left" colspan="2"><select class="rteInput" id="' + t.rteID + '_widthTypeInput"><option value="percent" selected>' + lang['TablePercent'] + '</option><option value="pixels">' + lang['TablePx'] + '</option></select></td></tr><tr><td class="rteModeText" align="right" nowrap>' + lang['TableBorderAbbr'] + ':&nbsp;</td><td><input class="rteInput" type="text" id="' + t.rteID + '_borderInput" value="0" size="4"></td><td class="rteModeText" align="left" colspan="2">' + lang['TablePx'] + '</td></tr><tr><td class="rteModeText" align="right">' + lang['TablePaddingAbbr'] + ':&nbsp;</td><td><input class="rteInput" type="text" id="' + t.rteID + '_paddingInput" value="4" size="4"></td><td class="rteModeText" align="right" nowrap>&nbsp;' + lang['TableSpacingAbbr'] + ':&nbsp;</td><td><input class="rteInput" type="text" id="' + t.rteID + '_spacingInput" value="0" size="4"></td></tr></table><div style="text-align: center;margin:5"><button type="button" id="' + t.rteID + '_tableBtn" class="rteBtn">' + lang['InsertTable'] + '</button></div></td></tr></table></div><div class="rteModeText" style="color:red">' + lang['SideMenuAlert'] + '</div></div></td>');

	document.getElementById(t.rteID + '_linkBtn').onclick = function() {
		var range = t.getRange();
		if (ua.ie || ua.opera) {
			text = stripHTML(range.text);
		} else {
			text = stripHTML(range.toString());
		}
		var url = document.getElementById(t.rteID + '_linkInput').value;
		if (url.length && text.length) {
			var link = '<a href="' + url + '">' + text + '</a>';
			t.insertHTML(link);
		}
	};
	document.getElementById(t.rteID + '_imageBtn').onclick = function() {
		t.focus();
		var path = document.getElementById(t.rteID + '_imageInput').value;
		if ((path != null) && (path != "")) {
			t.getWindow().document.execCommand('InsertImage',false , path);
		}
	};
	document.getElementById(t.rteID + '_tableBtn').onclick = function() {
		var rows = getInt(document.getElementById(t.rteID + '_rowsInput').value);
		var cols = getInt(document.getElementById(t.rteID + '_colsInput').value);
		var twidth = getInt(document.getElementById(t.rteID + '_widthInput').value);
		var twidthType = getInt(document.getElementById(t.rteID + '_widthTypeInput').value);
		var border = getInt(document.getElementById(t.rteID + '_borderInput').value);
		var padding = getInt(document.getElementById(t.rteID + '_paddingInput').value);
		var spacing = getInt(document.getElementById(t.rteID + '_spacingInput').value);
		if (rows > 0 && cols > 0) {
			var table = '<table cellpadding="' + padding + '" cellspacing="' + spacing + '" border="' + border + '" width="' + twidth + (twidthType == 'pixels' ? '' : '%') + '">';
			var i = 0;
			while (i < rows) {
				table += '<tr>';
				var j = 0;
				while (j < cols) {
					table += '<td></td>';
					j++;
				}
				table += '</tr>';
				i++;
			}
			table += '</table>';
			t.focus();
			t.insertHTML(table);
			t.showGuideLines();
		}		
	};
}

function changeMode(mode) {
	var t = this;
	var design = document.getElementById(t.rteID + '_designMode');
	var source = document.getElementById(t.rteID + '_sourceMode');
	if (t.sideMenu) {
		var sideMenu = document.getElementById(t.rteID + '_sideMenuDiv');
	}

	t.updateTextarea();

	switch(mode) {
		case 'design':
			if (design.className == 'rteMode') {
				t.mode = 'design';
				design.className = 'rteModeOn';
				source.className = 'rteMode';
				t.toggleHTMLSrc();
			}
			break;
		case 'source':
			if (source.className == 'rteMode') {
				t.mode = 'source';
				source.className = 'rteModeOn';
				design.className = 'rteMode';
				if (t.sideMenu && sideMenu.style.display == 'block') t.toggleSideMenu();
				t.toggleHTMLSrc();
			}
			break;
	}
};

function focus() {
	this.getWindow().focus();
};

function execCommand(cmd, op) {
	var t = this;
	var w = t.getWindow();
	for (var i = 0; i < allRTEs.length; i++) {
		allRTEs[i].hideDropMenu();
	}
	var page;
	var openDialog = false;
	var width;
	var height;

	switch (cmd) {
		case 'ForeColor':
		case 'HiliteColor':
			t.dlgColorPalette(cmd);
			break;
		case 'Print':
			if (ua.ie) {
				w.document.execCommand('Print');
			} else {
				w.print();
			}
			break;
		case 'Save':
			t.getTextarea().form.onsubmit();
			break;
		case 'HyperLink':
			openDialog = true;
			page = 'hyperlink.htm';
			width = (ua.ie) ? 420 : 424;
			height = (ua.ie) ? 196 : 200;
			break;
		case 'InsertImage':
			openDialog = true;
			page = 'image.htm';
			width = (ua.ie) ? 450 : 454;
			height = (ua.ie) ? 142 : 146;
			break;
		case 'InsertTable':
			openDialog = true;
			page = 'table.htm';
			width = (ua.ie) ? 350 : 354;
			height = (ua.ie) ? 140 : 144;
			break;
		case 'SpellCheck':
			t.checkSpell();
			break;
		case 'Search':
			openDialog = true;
			page = 'search.htm';
			width = (ua.ie) ? 350 : 354;
			height = (ua.ie) ? 160 : 164;
			break;
		case 'Preview':
			openDialog = true;
			page = 'preview.htm';
			width = 800;
			height = 600;
			break;
		case 'SpecialChar':
			openDialog = true;
			page = 'specialchar.htm';
			width = (ua.ie) ? 420 : 424;
			height = (ua.ie) ? 135 : 139;
			break;
		case 'Smilies':
			openDialog = true;
			page = 'smilies.htm';
			break;
		case 'PastePlainText':
			openDialog = true;
			page = 'paste_text.htm';
			width = (ua.ie) ? 330 : 350;
			height = (ua.ie) ? 345 : 358;
			break;
		case 'PasteFromWord':
			openDialog = true;
			page = 'paste_text.htm';
			width = (ua.ie) ? 330 : 350;
			height = (ua.ie) ? 350 : 358;
			break;
		case 'PasteAsHtml':
			break;
		case 'WordCount':
			t.countWords();
			break;
		default:
			try {
				t.focus();
				w.document.execCommand(cmd, false, op);
				t.focus();
				t.updateToolbarState();
			} catch (ex) {}
			break;
	}
	if (openDialog) {
		t.updateTextarea();
		dialogArgs = {
			'rte': t,
			'range': t.getRange(),
			'cssFile': t.cssFile,
			'cmd': cmd
		};
		t.popUpWin(basePath + page, '', width, height);
	}
};

function enableDesignMode() {
	var t = this;
	try {
		var d = t.getWindow().document;
	} catch (ex) {
		setTimeout(function() {t.enableDesignMode();}, 50);
	}
	try {
		d.designMode = 'on';
	} catch (ex) {
		setTimeout(function() {t.enableDesignMode();}, 50);
	}

	var html = '<html><head>';
			if (t.cssFile && t.cssFile.length) {
				html += '<link type="text/css" href="' + t.cssFile + '" rel="stylesheet">';
			} else {
				html += '<style>body {background: #FFFFFF;margin: 0px;padding: 0px;}</style>';
			}
			html += '</head><body>' + t.getTextarea().value + '</body></html>';

	try {
		d.open();
		d.write(html);
		d.close();
	} catch (ex) {
		setTimeout(function() {t.enableDesignMode();}, 50);
	}

	if (!t.readOnly) {
		try {
			ua.ie ? d.attachEvent('onkeypress', function(evt) {t.ieKeyPress(evt);}) : d.addEventListener('keypress', function(evt) {t.geckoKeyPress(evt);}, true);
			ua.ie ? d.attachEvent('onclick', function(evt) {t.hideDropMenu(); t.updateToolbarState(evt);}) : d.addEventListener('click', function(evt) {t.hideDropMenu(); t.updateToolbarState(evt);}, true);
			ua.ie ? d.attachEvent('onkeyup', function(evt) {t.updateToolbarState(evt);}) : d.addEventListener('keyup', function(evt) {t.updateToolbarState(evt);}, true);
		} catch (ex) {
			setTimeout(function() {t.enableDesignMode();}, 200);
		}
	}
	if (t.enableChangeMode) {
		document.getElementById(t.rteID + '_designMode').className = 'rteModeOn';
	}
};

function modifyOnSubmit() {
	var t = this;
	var ta = t.getTextarea();
	var form = ta.form;
	var onsubmit = form.onsubmit;
	if (typeof form.onsubmit != 'function') {
		form.onsubmit = function() {t.updateTextarea(); return true;};
	} else {
		form.onsubmit = function() {t.updateTextarea(); return onsubmit();};
	}
};

function showGuideLines() {
	var t = this;
	var d = t.getWindow().document;

  var tables = d.getElementsByTagName('TABLE');
	for (var i = 0; i < tables.length; i++) {
		if (tables[i] && getInt(tables[i].border) == 0) {
			tables[i].className = 'rteZeroBorder';
			tables[i].removeAttribute('border');
			tables[i].style.cssText = 'BORDER-RIGHT: #c0c0c0 1px dashed;	BORDER-TOP: #c0c0c0 1px dashed;	BORDER-LEFT: #c0c0c0 1px dashed;	BORDER-BOTTOM: #c0c0c0 1px dashed;	BORDER-COLLAPSE: separate;	BACKGROUND-COLOR: white;	border-spacing: 2px';
			var ths = tables[i].getElementsByTagName('TH');
			for (var j = 0; j < ths.length; j++) {
				ths[j].style.cssText = 'BORDER-RIGHT: #c0c0c0 1px dashed;	PADDING-RIGHT: 1px;	BORDER-TOP: #c0c0c0 1px dashed;	PADDING-LEFT: 1px;	PADDING-BOTTOM: 1px;	BORDER-LEFT: #c0c0c0 1px dashed;	PADDING-TOP: 1px;	BORDER-BOTTOM: #c0c0c0 1px dashed;';
			}
			var tds = tables[i].getElementsByTagName('TD');
			for (var j = 0; j < tds.length; j++) {
				tds[j].style.cssText = 'BORDER-RIGHT: #c0c0c0 1px dashed;	PADDING-RIGHT: 1px;	BORDER-TOP: #c0c0c0 1px dashed;	PADDING-LEFT: 1px;	PADDING-BOTTOM: 1px;	BORDER-LEFT: #c0c0c0 1px dashed;	PADDING-TOP: 1px;	BORDER-BOTTOM: #c0c0c0 1px dashed;';
			}
		}
	}
};

function hideGuideLines() {
	var t = this;
	var d = t.getWindow().document;
  var tables = d.getElementsByTagName('TABLE');
	for (var i = 0; i < tables.length; i++) {
		if (tables[i].className == 'rteZeroBorder') {
			tables[i].removeAttribute('style');

			var ths = tables[i].getElementsByTagName('TH');
			for (var j = 0; j < ths.length; j++) {
				ths[j].removeAttribute('style');
			}

			var tds = tables[i].getElementsByTagName('TD');
			for (var j = 0; j < tds.length; j++) {
				tds[j].removeAttribute('style');
			}
			tables[i].border = 0;
			if (ua.ie) {
				tables[i].removeAttribute('className');
			} else {
				tables[i].removeAttribute('class');
			}
		}
	}
};

function updateTextarea() {
	var t = this;
	var ta = t.getTextarea();
	var d = t.getWindow().document;
	if (t.mode == 'design') {
		t.hideGuideLines();
		if (t.generateXHTML) {
			ta.value = get_xhtml(d.body, language, encoding);
		} else {
			ta.value = d.body.innerHTML;
		}
		ta.value = replaceSpecialChars(ta.value);
		//if there is no content (other than formatting) set value to nothing
		if (stripHTML(ta.value.replace("&nbsp;", " ")) == "" &&
				ta.value.toLowerCase().search("<hr") == -1 &&
				ta.value.toLowerCase().search("<img") == -1) {
			ta.value = "";
		}
		t.showGuideLines();
	} else {
		if (ua.ie) {
			ta.value = d.body.innerText;
		} else {
			var src = d.createRange();
			src.selectNodeContents(d.body);
			ta.value = src.toString();
		}
	}
};

function replaceSpecialChars(html) { 
   var specials = new Array("&cent;","&euro;","&pound;","&curren;","&yen;","&copy;","&reg;","&trade;","&divide;","&times;","&plusmn;","&frac14;","&frac12;","&frac34;","&deg;","&sup1;","&sup2;","&sup3;","&micro;","&laquo;","&raquo;","&lsquo;","&rsquo;","&lsaquo;","&rsaquo;","&sbquo;","&bdquo;","&ldquo;","&rdquo;","&iexcl;","&brvbar;","&sect;","&not;","&macr;","&para;","&middot;","&cedil;","&iquest;","&fnof;","&mdash;","&ndash;","&bull;","&hellip;","&permil;","&ordf;","&ordm;","&szlig;","&dagger;","&Dagger;","&eth;","&ETH;","&oslash;","&Oslash;","&thorn;","&THORN;","&oelig;","&OElig;","&scaron;","&Scaron;","&acute;","&circ;","&tilde;","&uml;","&agrave;","&aacute;","&acirc;","&atilde;","&auml;","&aring;","&aelig;","&Agrave;","&Aacute;","&Acirc;","&Atilde;","&Auml;","&Aring;","&AElig;","&ccedil;","&Ccedil;","&egrave;","&eacute;","&ecirc;","&euml;","&Egrave;","&Eacute;","&Ecirc;","&Euml;","&igrave;","&iacute;","&icirc;","&iuml;","&Igrave;","&Iacute;","&Icirc;","&Iuml;","&ntilde;","&Ntilde;","&ograve;","&oacute;","&ocirc;","&otilde;","&ouml;","&Ograve;","&Oacute;","&Ocirc;","&Otilde;","&Ouml;","&ugrave;","&uacute;","&ucirc;","&uuml;","&Ugrave;","&Uacute;","&Ucirc;","&Uuml;","&yacute;","&yuml;","&Yacute;","&Yuml;"); 
   var unicodes = new Array("\u00a2","\u20ac","\u00a3","\u00a4","\u00a5","\u00a9","\u00ae","\u2122","\u00f7","\u00d7","\u00b1","\u00bc","\u00bd","\u00be","\u00b0","\u00b9","\u00b2","\u00b3","\u00b5","\u00ab","\u00bb","\u2018","\u2019","\u2039","\u203a","\u201a","\u201e","\u201c","\u201d","\u00a1","\u00a6","\u00a7","\u00ac","\u00af","\u00b6","\u00b7","\u00b8","\u00bf","\u0192","\u2014","\u2013","\u2022","\u2026","\u2030","\u00aa","\u00ba","\u00df","\u2020","\u2021","\u00f0","\u00d0","\u00f8","\u00d8","\u00fe","\u00de","\u0153","\u0152","\u0161","\u0160","\u00b4","\u02c6","\u02dc","\u00a8","\u00e0","\u00e1","\u00e2","\u00e3","\u00e4","\u00e5","\u00e6","\u00c0","\u00c1","\u00c2","\u00c3","\u00c4","\u00c5","\u00c6","\u00e7","\u00c7","\u00e8","\u00e9","\u00ea","\u00eb","\u00c8","\u00c9","\u00ca","\u00cb","\u00ec","\u00ed","\u00ee","\u00ef","\u00cc","\u00cd","\u00ce","\u00cf","\u00f1","\u00d1","\u00f2","\u00f3","\u00f4","\u00f5","\u00f6","\u00d2","\u00d3","\u00d4","\u00d5","\u00d6","\u00f9","\u00fa","\u00fb","\u00fc","\u00d9","\u00da","\u00db","\u00dc","\u00fd","\u00ff","\u00dd","\u0178"); 
   for (var i = 0; i < specials.length; i++) { 
      html = replaceIt(html, unicodes[i], specials[i]); 
   } 
   return html; 
};

function toggleHTMLSrc() {
	var t = this;
	var iframe = document.getElementById(t.rteID + '_iframe');
	var d = iframe.contentWindow.document;

	if (t.tb.show) {
		for (i = 1; i <= t.tb.line; i++) {
			document.getElementById(t.rteID + "_tbLine_" + i).style.display = (t.mode == 'design') ? 'block' : 'none';
		}
	}
	try {
		d.designMode = 'on';
	} catch (ex) {
		setTimeout(function() {t.toggleHTMLSrc();}, 50);
	}
	var height = getInt(iframe.style.height);
	if (t.mode == 'source') { // change to source mode
		if (t.tb.show) {
			iframe.style.height = (height + (t.tb.line * t.tb.lineHeight)) + 'px';
		}
		var ta = t.getTextarea();
		t.hideGuideLines();
		if (ua.ie) {
			d.body.innerText = ta.value;
		} else {

			//var htmlType = 'html|body|head|title|style|script|table|tr|th|tbody|thead|td|p|div|br|h1|h2|h3|h4|h5|h6';
			//var reg = new RegExp('(</(' + htmlType + ')( /)?>)', 'ig');

			//var text = ta.value.replace(reg, '$1\n');
			var text = ta.value;

			var src = d.createTextNode(text);
			d.body.innerHTML = '';
			d.body.appendChild(src);
			//d.body.innerHTML = d.body.innerHTML.replace('\n', '<br />');
		}
	} else { // change to design mode
		if (t.tb.show) {
			iframe.style.height = (height - (t.tb.line * t.tb.lineHeight)) + 'px';
		}
		if (ua.ie) {
			var output = escape(d.body.innerText);
			output = output.replace("%3CP%3E%0D%0A%3CHR%3E", "%3CHR%3E");
			output = output.replace("%3CHR%3E%0D%0A%3C/P%3E", "%3CHR%3E");
			d.body.innerHTML = unescape(output);
			// Disabled due to flaw in the regular expressions, this fix 
			// does not work with the revamped's enhanced insert link dialog window.
			//
			// Prevent links from changing to absolute paths
			if (!t.keepAbsolute) {
				var tagfix = unescape(output).match(/<a[^>]*href=(['"])([^\1>]*)\1[^>]*>/ig); 
				var coll = d.getElementsByTagName('A'); 
				for (var i = 0; i < coll.length; i++) { 
					// the 2 alerts below show when we hinder the links from becoming absolute			
					//alert(tagfix[i]); 
					coll[i].href = tagfix[i].replace(/.*href=(['"])([^\1]*)\1.*/i,"$2"); 
					//alert(RegExp.$1 + " " + RegExp.$2 + " " + RegExp.$3); 
				} 
				var imgfix = unescape(output).match(/<img[^>]*src=['"][^'"]*['"][^>]*>/ig); 
				var coll2 = d.getElementsByTagName('IMG'); 
				for (var i = 0; i < coll2.length; i++) { 
					coll2[i].src = imgfix[i].replace(/.*src=['"]([^'"]*)['"].*/i,"$1"); 
				} 
			}
			//end path fix			
		} else {
			var src = d.createRange();
			src.selectNodeContents(d.body);
			d.body.innerHTML = src.toString();
		}
		d.body.innerHTML = replaceSpecialChars(d.body.innerHTML);
		t.showGuideLines();
	}
};

function hideDropMenu() {
	var t = this;
	if (!t.tb.show) return;
	var palette = t.getPalette();
	if (typeof palette == 'object' && palette.style.visibility == 'visible') {
		palette.style.visibility = 'hidden';
	}
};

function dlgColorPalette(cmd) {
	//function to display or hide color palettes
	var t = this;
	var palette = t.getPalette();
	var range = t.getRange();
	var d = t.getWindow().document;
	
	//get dialog position
	var el = document.getElementById(t.rteID + '_' + cmd);
	var iLeftPos = getOffsetLeft(el);
	if (!ua.opera) {
		var iTopPos = getOffsetTop(el) + (el.offsetHeight + 4);
	} else {
		var iTopPos = el.offsetHeight + t.tb.iconHeight + 4;
	}
	palette.style.left = (iLeftPos) + "px";
	palette.style.top = (iTopPos) + "px";

	palette.contentWindow.dialogArgs = {
		'rte': t,
		'cmd': cmd,
		'range': range
	};
	palette.style.visibility = 'visible';
};

function popUpWin (url, win, width, height) {
	if (typeof dialog == 'object' && !dialog.closed) {
		var trueFalse = confirm(lang['DialogAlreadyOpen']);
		if (!trueFalse) return;
		dialog.close();
	}
	var left = (screen.availWidth - width) / 2;
	var top = (screen.availHeight - height) / 2;
	var options = 'width=' + width + ',height=' + height + ',left=' + left + ',top=' + top + ',status=no,resize=no';
	dialog = window.open(url, win, options);
}

function getOffsetTop(el) {
	var top = el.offsetTop;
	var p = el.offsetParent;
	var pCnt = 2;
	
	while(pCnt) {
		top += p.offsetTop;
		p = p.offsetParent;
		pCnt--;
	}
	return top;
}

function getOffsetLeft(el) {
	var left = el.offsetLeft;
	var p = el.offsetParent;
	var pCnt = 2;
	
	while(pCnt) {
		left += p.offsetLeft;
		p = p.offsetParent;
		pCnt--;
	}
	return left;
}

function selectFont(cmd, value) {
	var t = this;
	//t.hideDropMenu();
	var d = t.getWindow().document;
	d.execCommand(cmd, false, value);
}

function countWords() {
	var t = this;
	var maxchar = 64000;
	
	t.updateTextarea();
  var words = t.getTextarea().value;
  var str = stripHTML(words);
  var chars = trim(words);
  chars = chars.length;
  chars = maxchar - chars;
  str = str + " a ";  // word added to avoid error
  str = str.replace(/&nbsp;/gi,' ').replace(/([\n\r\t])/g,' ').replace(/(  +)/g,' ').replace(/&(.*);/g,' ').replace(/^\s*|\s*$/g,'');
  var count = 0;
  for (var x = 0; x < str.length; x++) {
    if (str.charAt(x) == " " && str.charAt(x-1) != " ") count++;  
  }
  if (str.charAt(str.length-1) != " ") count++;
	count = count - 1; // extra word removed
	var alarm = "";
  if(chars < 0) alarm = "\n\n" + lang['CountCharWarn'];
  alert(lang['CountTotal'] + ": " + count +  "\n\n" + lang['CountChar'] + ": " + chars + alarm);
}

function insertHTML(html) {
	//function to add HTML -- thanks dannyuk1982
	var t = this;
	var d = t.getWindow().document;

	t.focus();
	if (ua.ie) {
		var selection = d.selection;
		var range = selection.createRange();
		range.pasteHTML(html);
		range.collapse(false);
		range.select();
	} else if (ua.opera) {
		alert('not supported yet');
	} else {
		d.execCommand('insertHTML', false, html);
	}
};

function getRange() {
	//function to store range of current selection
	var t = this;
	var w = t.getWindow();
	if (ua.ie || ua.opera) {
		return (w.document.selection != null) ? w.document.selection.createRange() : null;
	} else {
		try {
			var selection = w.getSelection();
			return (selection != null) ? selection.getRangeAt(selection.rangeCount - 1).cloneRange() : null;
		} catch (ex) {return null;}
	}
};

function stripHTML(oldString) {
	//function to strip all html
	var newString = oldString.replace(/(<([^>]+)>)/ig,"");
	
	//replace carriage returns and line feeds
   newString = newString.replace(/\r\n/g," ");
   newString = newString.replace(/\n/g," ");
   newString = newString.replace(/\r/g," ");
	
	//trim string
	newString = trim(newString);
	
	return newString;
}

function trim(inputString) {
   // Removes leading and trailing spaces from the passed string. Also removes
   // consecutive spaces and replaces it with one space. If something besides
   // a string is passed in (null, custom object, etc.) then return the input.
   if (typeof inputString != "string") return inputString;
   var retValue = inputString;
   var ch = retValue.substring(0, 1);
	
   while (ch == " ") { // Check for spaces at the beginning of the string
      retValue = retValue.substring(1, retValue.length);
      ch = retValue.substring(0, 1);
   }
   ch = retValue.substring(retValue.length - 1, retValue.length);
	
   while (ch == " ") { // Check for spaces at the end of the string
      retValue = retValue.substring(0, retValue.length - 1);
      ch = retValue.substring(retValue.length - 1, retValue.length);
   }
	
	// Note that there are two spaces in the string - look for multiple spaces within the string
   while (retValue.indexOf("  ") != -1) {
		// Again, there are two spaces in each of the strings
      retValue = retValue.substring(0, retValue.indexOf("  ")) + retValue.substring(retValue.indexOf("  ") + 1, retValue.length);
   }
   return retValue; // Return the trimmed string back to the user
}

//********************
//Gecko-Only Functions
//********************
function geckoKeyPress(evt) {
	//function to add bold, italic, and underline shortcut commands to gecko RTEs
	//contributed by Anti Veeranna (thanks Anti!)
	//var rte = evt.target.id;
	var t = this;
	
	if (evt.ctrlKey) {
		var key = String.fromCharCode(evt.charCode).toLowerCase();
		var cmd = null;
		switch (key) {
			case 'b': cmd = "bold"; break;
			case 'i': cmd = "italic"; break;
			case 'u': cmd = "underline"; break;
		};

		if (cmd) {
			var d = t.getWindow().document;
			d.execCommand(cmd, false, '');
			
			// stop the event bubble
			evt.preventDefault();
			evt.stopPropagation();
		}
 	}
};

//*****************
//IE-Only Functions
//*****************
function ieKeyPress(evt) {
	var key = (evt.which || evt.charCode || evt.keyCode);
	var stringKey = String.fromCharCode(key).toLowerCase();
/*	
//the following breaks list and indentation functionality in IE (don't use)
	switch (key) {
		case 13:
			//insert <div> tag instead of <p>
			//change the key pressed to null
			evt.keyCode = 0;
			
			//insert <br> tag
			var t = this;
			var d = document.getElementById(this.rteID + '_iframe').contentWindow.document;
			d.execCommand('FormatBlock', false, '<DIV>');
			break;
	}*/
};

function checkSpell() {
	//function to perform spell check
	try {
		var tmpis = new ActiveXObject("ieSpell.ieSpellExtension");
		tmpis.CheckAllLinkedDocuments(document);
	}
	catch(exception) {
		if(exception.number==-2146827859) {
			if (confirm("ieSpell not detected.  Click Ok to go to download page."))
				window.open("http://www.iespell.com/download.php","DownLoad");
		} else {
			alert("Error Loading ieSpell: Exception " + exception.number);
		}
	}
};

function updateToolbarState(e, dontStop) {
	var t = this;
	if (!t.tb.show) return false;
	var w = t.getWindow();

	// get rid of all styles
	for (var b in t.tb.itemObj) {
		if (b == 'FormatBlock' || b == 'FontName' || b == 'FontSize') {
			t.tb.itemObj[b].value = '';
			t.tb.itemObj[b].className = 'rteSelect';
		} else {
			t.tb.itemObj[b].className = 'rteImage';
		}
	}
	if (ua.ie || ua.opera) {
		var range = w.document.selection.createRange();
		try {
			var p = range.parentElement();
		} catch (ex) {
			return false;
		}
	} else {
		var selection = w.getSelection();
		try {
			var range = selection.getRangeAt(0);
		} catch (ex) {
			return false;
		}
		var p = range.commonAncestorContainer;
	}

	while (p.nodeType == 3) {//NODE.TEXT_NODE
		p = p.parentNode;
	}
	while (p && p.nodeName != "BODY" && p.nodeName != "HTML")
	{
		if (p.style) {
			// fontFamily, fontSize, fontWeight, fontStyle, fontDecoration, textAlign 
			if (p.style.fontFamily) {
				t.tb.itemObj['FontName'].value = p.style.fontFamily;
				t.tb.itemObj['FontName'].className = 'rteSelectDown';
			}
			if (p.style.fontSize) {
				t.tb.itemObj['FontSize'].value = p.style.fontSize;
				t.tb.itemObj['FontSize'].className = 'rteSelectDown';
			}
			if (p.style.fontWeight == 'bold') {
				t.tb.itemObj['Bold'].className = 'rteImageDown';
			}
			if (p.style.fontStyle == 'italic') {
				t.tb.itemObj['Italic'].className = 'rteImageDown';
			}
			if (p.style.textDecoration.indexOf('underline') != -1) {
				t.tb.itemObj['Underline'].className = 'rteImageDown';
			}
			if (p.style.textDecoration.indexOf('line-through') != -1) {
				t.tb.itemObj['StrikeThrough'].className = 'rteImageDown';
			}
			if (p.style.textAlign) {
				switch(p.style.textAlign) {
					case 'left':
						t.tb.itemObj['JustifyLeft'].className = 'rteImageDown';
						break;
					case 'right':
						t.tb.itemObj['JustifyRight'].className = 'rteImageDown';
						break;
					case 'center':
						t.tb.itemObj['JustifyCenter'].className = 'rteImageDown';
						break;
					case 'justify':
						t.tb.itemObj['JustifyFull'].className = 'rteImageDown';
						break;
				}
			}
			if (p.style.verticalAlign) {
				switch (p.style.verticalAlign) {
					case 'sup':
						t.tb.itemObj['Superscript'].className = 'rteImageDown';
						break;
					case 'sub':
						t.tb.itemObj['Subscript'].className = 'rteImageDown';
						break;
				}
			}
		}
		switch (p.nodeName) {
			case 'A':
				if (p.name) {
/*					t.tb.itemObj['Bookmark'].className = 'rteImageDown'; */
				} else {
					t.tb.itemObj['HyperLink'].className = 'rteImageDown';
				}
				break;
			case 'I':
			case 'EM':
				t.tb.itemObj['Italic'].className = 'rteImageDown';
				break;
			case 'LI':
				if (p.parentNode.nodeName == 'OL') {
					t.tb.itemObj['InsertOrderedList'].className = 'rteImageDown';
				} else if (p.parentNode.nodeName == 'UL') {
					t.tb.itemObj['InsertUnorderedList'].className = 'rteImageDown';
				}
				break;
			case 'B':
			case 'STRONG':
				t.tb.itemObj['Bold'].className = 'rteImageDown';
				break;
			case 'SUB':
				t.tb.itemObj['Subscript'].className = 'rteImageDown';
				break;
			case 'SUP':
				t.tb.itemObj['Superscript'].className = 'rteImageDown';
				break;
			case 'FONT':
				if (p.size) {
					t.tb.itemObj['FontSize'].value = p.size;
					t.tb.itemObj['FontSize'].className = 'rteSelectDown';
				}
				if (p.face) {
					t.tb.itemObj['FontName'].value = p.face;
					t.tb.itemObj['FontName'].className = 'rteSelectDown';
				}
				break;
			case 'S':
			case 'STRIKE':
				t.tb.itemObj['StrikeThrough'].className = 'rteImageDown';
				break;
			case 'U':
				t.tb.itemObj['Underline'].className = 'rteImageDown';
				break;
			case 'DIV':
			case 'P':
					t.tb.itemObj['FormatBlock'].value = '<' + p.nodeName + '>';
					t.tb.itemObj['FormatBlock'].className = 'rteSelectDown';
				switch(p.align) {
					case 'left':
						t.tb.itemObj['JustifyLeft'].className = 'rteImageDown';
						break;
					case 'right':
						t.tb.itemObj['JustifyRight'].className = 'rteImageDown';
						break;
					case 'center':
						t.tb.itemObj['JustifyCenter'].className = 'rteImageDown';
						break;
					case 'justify':
						t.tb.itemObj['JustifyFull'].className = 'rteImageDown';
						break;
				}
				break;
			case 'H1':
			case 'H2':
			case 'H3':
			case 'H4':
			case 'H5':
			case 'H6':
			case 'ADDRESS':
			case 'PRE':
				t.tb.itemObj['FormatBlock'].value = '<' + p.nodeName + '>';
				t.tb.itemObj['FormatBlock'].className = 'rteSelectDown';
				break;
/*			case 'TABLE':
				t.tb.itemObj['Table'].className = 'rteImageDown';
				break; */
		}
		p = p.parentNode;
	}
	if (!dontStop && e) {
		if (ua.ie) {
			e.cancelBubble = true;
			e.returnValue = false;
		} else {
			e.preventDefault();
			e.stopPropagation();
		}
	}
};

function autoBRon(id) {
  // CM 19/10/04 used for non RTE browsers to deal with auto <BR> (and clean up other muck)
	var forms = document.forms;
	var ta;

	for (var i in forms) {
		for (var j in forms[i]) {
			var form = forms[i];
			if (form.elements[id]) {
				ta = form.elements[id];
				break;
			}
		}
	}
	if (ta) {
		ta.value = parseBreaks(ta.value);
		ta.value = replaceIt(ta.value,'&apos;','\'');
	}
};

function autoBRoff(id) {
  // CM 19/10/04 used for non RTE browsers to deal with auto <BR> (auto carried out when the form is submitted)
	var forms = document.forms;
	var ta;

	for (var i in forms) {
		for (var j in forms[i]) {
			var form = forms[i];
			if (form.elements[id]) {
				ta = form.elements[id];
				break;
			}
		}
	}
	if (ta) {
		ta.value = replaceIt(ta.value,'\n','<br />');
		ta.value = replaceIt(ta.value,'\'','&apos;');
	}
};

function replaceIt(string,text,by) {
  // CM 19/10/04 custom replace function
  var strLength = string.length, txtLength = text.length;
  if ((strLength == 0) || (txtLength == 0)) return string;
  var i = string.indexOf(text);
  if ((!i) && (text != string.substring(0,txtLength))) return string;
  if (i == -1) return string;
  var newstr = string.substring(0,i) + by;
  if (i+txtLength < strLength)
    newstr += replaceIt(string.substring(i+txtLength,strLength),text,by);
  return newstr;
};

function parseBreaks(argIn) {
  // CM 19/10/04 used for non RTE browsers to deal with auto <BR> (and clean up other muck)
	argIn=replaceIt(argIn,'<br>','\n');
	argIn=replaceIt(argIn,'<BR>','\n');
	argIn=replaceIt(argIn,'<br/>','\n');
	argIn=replaceIt(argIn,'<br />','\n');
	argIn=replaceIt(argIn,'\t',' ');
	argIn=replaceIt(argIn,'    ',' ');
	argIn=replaceIt(argIn,'   ',' ');
	argIn=replaceIt(argIn,'  ',' ');
	argIn=replaceIt(argIn,'  ',' ');
	argIn=replaceIt(argIn,'\n ','\n');
	argIn=replaceIt(argIn,' <p>','<p>');
	argIn=replaceIt(argIn,'</p><p>','\n\n');
	argIn=replaceIt(argIn,'&apos;','\'');
	return argIn;
}
