<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format">
	<xsl:output method="html"/>
	<xsl:template match="/">
		<xsl:variable name="locale">
			<xsl:choose>
				<xsl:when test="string-length(/admin/data/locale/text()) &gt; 0">
					<xsl:value-of select="/admin/data/locale"/>
				</xsl:when>
				<xsl:otherwise>Default</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>

		<html>
			<head>
				<title>Website</title>
				<style type="text/css">
IMG.folder { padding:1px 4px 1px 1px; color:red}				
A.folder_static { padding:1px 4px 1px 1px; color:blue; }				
A.folder_dynamic { padding:1px 4px 1px 1px; color:green; }				
A.folder_both { padding:1px 4px 1px 1px; color:cyan; }				
IMG.file { padding:1px 4px 1px 1px; }				
A.file_static { color:blue; }
A.file_dynamic { color:green; }
A.file_overlay { color:cyan; }
				</style>
			</head>
			<body>
				<font size="+2"><b><xsl:value-of select="/admin/data/base"/></b></font>
				<img alt=" --- " src="/images/blank_16x16.png"/>
				<span style="font-weight:lighter; font-size:small; font-family:arial;"><xsl:text> ( </xsl:text>
				LOCALE: <xsl:value-of select="$locale"/>
					<xsl:text> {</xsl:text>
					<a>
						<xsl:attribute name="href">/admin?command=website_list&amp;locale=en-us</xsl:attribute>
						<xsl:text>en-us</xsl:text>
					</a>
					<xsl:text>,</xsl:text>
					<a>
						<xsl:attribute name="href">/admin?command=website_list&amp;locale=en-gb</xsl:attribute>
						<xsl:text>en-gb</xsl:text>
					</a>
					<xsl:text>,</xsl:text>
					<a>
						<xsl:attribute name="href">/admin?command=website_list&amp;locale=en-pirate</xsl:attribute>
						<xsl:text>en-pirate</xsl:text>
					</a>
					<xsl:text>,</xsl:text>
					<a>
						<xsl:attribute name="href">/admin?command=website_list&amp;locale=ru-ru</xsl:attribute>
						<xsl:text>ru-ru</xsl:text>
					</a>
				<xsl:text>}, ROOT: </xsl:text><xsl:value-of select="/admin/data/root"/>
				<xsl:text> ) </xsl:text>
				</span>
				<span style="float:right; text-align:left; position:relative;"><a target="_top" href="/">Home</a></span>
				<br clear="all"/>
				<hr/>
				<xsl:apply-templates select="/admin/website"/>
			</body>
		</html>
	</xsl:template>
	
	<xsl:template match="website">
		<xsl:variable name="locale">
			<xsl:choose>
				<xsl:when test="string-length(/admin/data/locale/text()) &gt; 0">
					<xsl:value-of select="/admin/data/locale"/>
				</xsl:when>
			</xsl:choose>
		</xsl:variable>

		<xsl:if test="/admin/data/base != '/'">
			<img alt="+" title="back" src="/images/folder_open.gif" class="folder"/>
			<a>
				<xsl:attribute name="href"><xsl:value-of select="/admin/data/url"/>&amp;locale=<xsl:value-of select="$locale"/>&amp;basepath=<xsl:value-of select="/admin/data/base"/>../</xsl:attribute>
				<b>..</b>
			</a>
			<br/>
		</xsl:if>
		<xsl:apply-templates select="dir"/>
		<xsl:apply-templates select="file"/>
	</xsl:template>
	
	<xsl:template match="dir">
		<xsl:variable name="locale">
			<xsl:choose>
				<xsl:when test="string-length(/admin/data/locale/text()) &gt; 0">
					<xsl:value-of select="/admin/data/locale"/>
				</xsl:when>
			</xsl:choose>
		</xsl:variable>

		<span style="display:block">
			<xsl:choose>
				<xsl:when test="@type = 1">
					<img alt="+" title="static folder" src="/images/folder_static.gif" class="folder"/>
					<a class="folder_static">
						<xsl:attribute name="href"><xsl:value-of select="/admin/data/url"/>&amp;locale=<xsl:value-of select="$locale"/>&amp;basepath=<xsl:value-of select="/admin/data/base"/><xsl:value-of select="."/>/</xsl:attribute>
						<xsl:value-of select="."/>
					</a>
				</xsl:when>

				<xsl:when test="@type = 2">
					<img alt="+" title="dynamic folder" src="/images/folder_dynamic.gif" class="folder"/>
					<a class="folder_dynamic">
						<xsl:attribute name="href"><xsl:value-of select="/admin/data/url"/>&amp;locale=<xsl:value-of select="$locale"/>&amp;basepath=<xsl:value-of select="/admin/data/base"/><xsl:value-of select="."/>/</xsl:attribute>
						<xsl:value-of select="."/>
					</a>
				</xsl:when>

				<xsl:when test="@type = 3">
					<img alt="+" title="static and dynamic folder" src="/images/folder_both.gif" class="folder"/>
					<a class="folder_both">
						<xsl:attribute name="href"><xsl:value-of select="/admin/data/url"/>&amp;locale=<xsl:value-of select="$locale"/>&amp;basepath=<xsl:value-of select="/admin/data/base"/><xsl:value-of select="."/>/</xsl:attribute>
						<xsl:value-of select="."/>
					</a>
				</xsl:when>

				<xsl:otherwise>
					<img alt="+" title="folder?" src="/images/unknown.gif" class="folder"/>
				</xsl:otherwise>
			</xsl:choose>

		</span>
	</xsl:template>
	
	<xsl:template match="file">
		<xsl:variable name="locale">
			<xsl:choose>
				<xsl:when test="string-length(/admin/data/locale/text()) &gt; 0">
					<xsl:value-of select="/admin/data/locale"/>
				</xsl:when>
			</xsl:choose>
		</xsl:variable>

		<span style="display:block">
			<xsl:choose>
				<xsl:when test="@type = 4">
					<img title="static file" src="/images/file_static.png" class="file"/>
					<a class="file_static">
						<xsl:attribute name="href">/admin?command=website_view&amp;locale=<xsl:value-of select="$locale"/>&amp;static=<xsl:value-of select="/admin/data/base"/><xsl:value-of select="."/></xsl:attribute>
						<xsl:attribute name="target">_target</xsl:attribute>
						<xsl:value-of select="."/>
					</a>
				</xsl:when>

				<xsl:when test="@type = 8">
					<img title="dynamic controller" src="/images/controller.png" class="file"/>
					<a class="file_dynamic">
						<xsl:attribute name="href">/admin?command=website_view&amp;locale=<xsl:value-of select="$locale"/>&amp;dynamic=<xsl:value-of select="/admin/data/base"/><xsl:value-of select="."/>.aos.xml</xsl:attribute>
						<xsl:attribute name="target">_target</xsl:attribute>
						<xsl:value-of select="."/>
					</a>
				</xsl:when>

				<xsl:when test="@type = 12">
					<img title="dynamic controller overlaying static file" src="/images/controller_overlay.png" class="file"/>
					<a class="file_overlay">
						<xsl:attribute name="href">/admin?command=website_view&amp;locale=<xsl:value-of select="$locale"/>&amp;dynamic=<xsl:value-of select="/admin/data/base"/><xsl:value-of select="."/>.aos.xml</xsl:attribute>
						<xsl:attribute name="target">_target</xsl:attribute>
						<xsl:value-of select="."/>
					</a>
					<xsl:text> ( </xsl:text>
					<i><small>
						<a class="static">
							<xsl:attribute name="href">/admin?command=website_view&amp;locale=<xsl:value-of select="$locale"/>&amp;static=<xsl:value-of select="/admin/data/base"/><xsl:value-of select="."/></xsl:attribute>
							<xsl:attribute name="target">_target</xsl:attribute>
							<xsl:value-of select="."/>
						</a>
					</small></i>
					<xsl:text> )</xsl:text>
				</xsl:when>
				
				<xsl:otherwise>
					<img title="file?" src="/images/file.png" class="file"/>
				</xsl:otherwise>
			</xsl:choose>
		</span>
	</xsl:template>
</xsl:stylesheet>
