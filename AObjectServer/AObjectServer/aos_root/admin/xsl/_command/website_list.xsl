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
IMG.folder { padding:1px 4px 1px 1px; height:16px; width:20px; }				
IMG.folder_link { padding:0px 3px 0px 3px; height:12px; width:12px; border:0; }
A.folder { padding:1px 5px 1px 1px; color:#1010f0; font-size:large; font-weight:bolder; }				

IMG.file { padding:1px 4px 1px 1px; height:14px; width:18px; }				
IMG.file_link { padding:0px 3px 0px 3px; height:12px; width:12px; border:0; }				
A.file_first { color:#0000f0; font-weight:bolder; }
A.file { color:#808080; }

SPAN.folder_line { background-color:#ddddff; padding: 0px 0px 0px 0px; vertical-align:top; }
SPAN.file_line { background-color:#f0f0ff; }

SPAN.path { color:blue; font-face:Arial; font-weight:bolder; }
				</style>
			</head>
			<body>
				<div><span style="float:left;"><i>Base path: <font size="-1"><xsl:value-of select="/admin/data/root"/></font></i></span><span style="float:right; text-align:left;"><a target="_top" href="/">Home</a></span></div>
				<br clear="all"/>
				<form method="get">
					<input type="hidden" name="command" value="website_list"/>					
					<input type="hidden" name="path">
						<xsl:attribute name="value"><xsl:value-of select="/admin/data/base"/></xsl:attribute>
					</input>
					<table width="100%" border="1">
						<tr>
							<th>Path</th>
							<th>Locale</th>
						</tr>
						<tr>
							<td align="center" width="70%"><span class="path"><xsl:value-of select="/admin/data/base"/></span></td>
							<td align="center"><xsl:copy-of select="/admin/data/select_locale/select"/> <input type="submit" name="Set" value="Set"/></td>
						</tr>
					</table>
				</form>
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
				<xsl:attribute name="href"><xsl:value-of select="/admin/data/url"/>&amp;locale=<xsl:value-of select="$locale"/>&amp;path=<xsl:value-of select="/admin/data/base"/>../</xsl:attribute>
				<b>..</b>
			</a>
			<br/>
		</xsl:if>
		<xsl:apply-templates select="dir"/>
		<xsl:apply-templates select="file"/>
	</xsl:template>
	
	<!-- DIR -->
	<xsl:template match="dir">
		<xsl:variable name="locale">
			<xsl:choose>
				<xsl:when test="string-length(/admin/data/locale/text()) &gt; 0">
					<xsl:value-of select="/admin/data/locale"/>
				</xsl:when>
			</xsl:choose>
		</xsl:variable>
		<xsl:variable name="name" select="name/text()"/>

		<span style="display:block">
			<xsl:choose>
				<xsl:when test="@type = 1">
					<img alt="+" title="static folder" src="/images/folder_static.gif" class="folder"/>
				</xsl:when>

				<xsl:when test="@type = 2">
					<img alt="+" title="dynamic folder" src="/images/folder_dynamic.gif" class="folder"/>
				</xsl:when>

				<xsl:when test="@type = 3">
					<img alt="+" title="static and dynamic folder" src="/images/folder_both.gif" class="folder"/>
				</xsl:when>

				<xsl:otherwise>
					<img alt="+" title="folder?" src="/images/unknown.gif" class="folder"/>
				</xsl:otherwise>
			</xsl:choose>

			<span class="folder_line">
				<xsl:for-each select="path">
					<a class="folder">
						<xsl:attribute name="href"><xsl:value-of select="/admin/data/url"/>&amp;locale=<xsl:value-of select="$locale"/>&amp;path=<xsl:value-of select="/admin/data/base"/><xsl:value-of select="data"/></xsl:attribute>

						<xsl:choose>
							<xsl:when test="@type = 1">
								<xsl:attribute name="title"><xsl:value-of select="info"/></xsl:attribute>
							</xsl:when>
			
							<xsl:when test="@type = 2">
								<xsl:attribute name="title"><xsl:value-of select="info"/></xsl:attribute>
							</xsl:when>
			
							<xsl:otherwise>
								<xsl:attribute name="title"><xsl:value-of select="info"/></xsl:attribute>
							</xsl:otherwise>
						</xsl:choose>
						<xsl:value-of select="$name"/>
		
					</a>
					<xsl:text>  </xsl:text>
				</xsl:for-each>
			</span>
		</span>
	</xsl:template>
	
	<!-- FILE -->
	<xsl:template match="file">
		<xsl:variable name="locale">
			<xsl:choose>
				<xsl:when test="string-length(/admin/data/locale/text()) &gt; 0">
					<xsl:value-of select="/admin/data/locale"/>
				</xsl:when>
			</xsl:choose>
		</xsl:variable>
		<xsl:variable name="name" select="name/text()"/>

		<span style="display:block">
			<xsl:choose>
				<xsl:when test="@type = 4">
					<img title="static file" src="/images/file_static.png" class="file"/>
				</xsl:when>

				<xsl:when test="@type = 8">
					<img title="dynamic controller" src="/images/controller.png" class="file"/>
				</xsl:when>

				<xsl:when test="@type = 12">
					<img title="dynamic controller overlaying static file" src="/images/controller_overlay.png" class="file"/>
				</xsl:when>
				
				<xsl:otherwise>
					<img title="file?" src="/images/file.png" class="file"/>
				</xsl:otherwise>
			</xsl:choose>

			<span class="file_line">
				<xsl:for-each select="path">
					<xsl:element name="a">
						<xsl:if test="position() = 1">
							<xsl:attribute name="class">file_first</xsl:attribute>
						</xsl:if>
						<xsl:if test="position() > 1">
							<xsl:attribute name="class">file</xsl:attribute>
						</xsl:if>
						<xsl:attribute name="href">/admin?command=website_view&amp;locale=<xsl:value-of select="$locale"/>&amp;path=<xsl:value-of select="data"/></xsl:attribute>
						<xsl:attribute name="target">_target</xsl:attribute>

						<xsl:choose>
							<xsl:when test="@type = 3">
								<xsl:attribute name="title"><xsl:value-of select="info"/></xsl:attribute>
								<xsl:if test="last() > 1">
									<img src="/images/file_static.png" class="file_link"/>
								</xsl:if>
							</xsl:when>
			
							<xsl:when test="@type = 4">
								<xsl:attribute name="title"><xsl:value-of select="info"/></xsl:attribute>
								<xsl:if test="last() > 1">
									<img src="/images/controller.png" class="file_link"/>
								</xsl:if>
							</xsl:when>
							
							<xsl:otherwise>
								<xsl:attribute name="title"><xsl:value-of select="info"/></xsl:attribute>
								<img src="/images/file.png" class="file_link"/>
							</xsl:otherwise>
						</xsl:choose>

						<xsl:value-of select="$name"/>
					</xsl:element>
					<xsl:text>  </xsl:text>
				</xsl:for-each>
			</span>
		</span>
	</xsl:template>
</xsl:stylesheet>
