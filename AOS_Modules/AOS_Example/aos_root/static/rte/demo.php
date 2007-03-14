<html>
<head>
	<title>Cross-Browser Rich Text Editor</title>
	<script type="text/javascript" src="lang/en.js"></script>
	<script language="JavaScript" type="text/javascript" src="html2xhtml.js"></script>
	<script language="JavaScript" type="text/javascript" src="richtext.js"></script>
</head>
<body>

<!-- START Demo Code -->
<form name="RTEDemo" action="<?=$_SERVER["PHP_SELF"]?>" method="post">
<noscript><p><b>Javascript must be enabled to use this form.</b></p></noscript>

<script language="JavaScript" type="text/javascript">
<!--
<?php
//format content for preloading
if (!(isset($_POST["rte1"]))) {
	$content = "here's the " . chr(13) . "\"preloaded <b>content</b>\"";
	$content = rteSafe($content);
} else {
	//retrieve posted value
	$content = rteSafe($_POST["rte1"]);
}
?>
//Usage: fieldname, html, width, height, buttons, readOnly
var rte1 = new RichText('rte1', '<?php echo $content;?>', 520, 200, true, false);
rte1.create();
//-->
</script>

<p>Click submit to post the form and reload with your rte content.</p>
<p><input type="submit" name="submit" value="Submit"></p>
</form>
<?php
function rteSafe($strText) {
	//returns safe code for preloading in the RTE
	$tmpString = $strText;
	
	//convert all types of single quotes
	$tmpString = str_replace(chr(145), chr(39), $tmpString);
	$tmpString = str_replace(chr(146), chr(39), $tmpString);
	$tmpString = str_replace("'", "&#39;", $tmpString);
	
	//convert all types of double quotes
	$tmpString = str_replace(chr(147), chr(34), $tmpString);
	$tmpString = str_replace(chr(148), chr(34), $tmpString);
//	$tmpString = str_replace("\"", "\"", $tmpString);
	
	//replace carriage returns & line feeds
	$tmpString = str_replace(chr(10), " ", $tmpString);
	$tmpString = str_replace(chr(13), " ", $tmpString);
	
	return $tmpString;
}
?>
<!-- END Demo Code -->

</body>
</html>
