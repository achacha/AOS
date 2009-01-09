<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format">
	<xsl:output method="html"/>
	<xsl:template match="/">
		<html>
			<head>
				<title>Website</title>
				<style type="text/css">
IMG.folder { padding:1px 4px 1px 1px; }				
IMG.file { padding:1px 4px 1px 1px; }				
				</style>
			</head>
			<body>
				<font size="+1">WebSite</font><br/>
				<div>ROOT: <xsl:value-of select="/admin/data/root"/></div>
				<div>LOCALE: <xsl:choose>
					<xsl:when test="string-length(/admin/data/locale/text()) &gt; 0"><xsl:value-of select="/admin/data/locale"/></xsl:when>
					<xsl:otherwise>Default</xsl:otherwise>
				</xsl:choose></div>
				<b><xsl:value-of select="/admin/data/base"/></b><br/>
				<hr/>
				<xsl:apply-templates select="/admin/website"/>
			</body>
		</html>
	</xsl:template>

	<xsl:template match="website">
		<xsl:if test="/admin/data/base != '/'">
		<a>
			<xsl:attribute name="href"><xsl:value-of select="/admin/data/url"/>&amp;basepath=<xsl:value-of select="/admin/data/base"/>../</xsl:attribute>
			<b>..</b>
		</a>
		<br/>
		</xsl:if>
		<xsl:apply-templates select="dir"/>
		<xsl:apply-templates select="file"/>
	</xsl:template>

	<xsl:template match="dir">
		<span style="display:block">
			<xsl:choose>
				<xsl:when test="@type = 1"><img alt="+" title="static folder" src="/images/folder_static.gif" class="folder"/></xsl:when>
				<xsl:when test="@type = 2"><img alt="+" title="dynamic folder" src="/images/folder_dynamic.gif" class="folder"/></xsl:when>
				<xsl:when test="@type = 3"><img alt="+" title="static and dynamic folder" src="/images/folder_both.gif" class="folder"/></xsl:when>
				<xsl:otherwise><img alt="+" title="folder?" src="/images/unknown.gif" class="folder"/></xsl:otherwise>
			</xsl:choose>
			<a>
				<xsl:attribute name="href"><xsl:value-of select="/admin/data/url"/>&amp;basepath=<xsl:value-of select="/admin/data/base"/><xsl:value-of select="."/>/</xsl:attribute>
				<xsl:value-of select="."/>
			</a>
		</span>
	</xsl:template>

	<xsl:template match="file">
		<span style="display:block">
			<xsl:choose>
				<xsl:when test="@type = 4"><img title="static file" src="/images/file_static.png" class="file"/></xsl:when>
				<xsl:when test="@type = 8"><img title="dynamic controller" src="/images/controller.png" class="file"/></xsl:when>
				<xsl:when test="@type = 12"><img title="dynamic controller overlaying static file" src="/images/controller_overlay.png" class="file"/></xsl:when>
				<xsl:otherwise><img title="file?" src="/images/file.png" class="file"/></xsl:otherwise>
			</xsl:choose>
			<xsl:value-of select="."/>
		</span>
	</xsl:template>

</xsl:stylesheet>
