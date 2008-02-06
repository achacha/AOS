<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format">
	<xsl:output method="html"/>
	<xsl:template match="/root">
		<html>
			<head>
				<title></title>
			</head>
			<body>
				<xsl:apply-templates select="dada/line"/>
				<br/>
				<div align="center"><font size="1">Request execution time is <xsl:value-of select="/root/request_time"/> milliseconds.</font></div>
			</body>
		</html>
	</xsl:template>

	<xsl:template match="line">
		<xsl:choose>
			<xsl:when test="@type='title'">
				<h1>
					<xsl:apply-templates select="image"/>
					<xsl:copy-of select="."/>
				</h1>
			</xsl:when>
			<xsl:otherwise>
				<xsl:apply-templates select="image"/>
				<xsl:copy-of select="."/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	
	<xsl:template match="image">
		<img src="{@url}" alt="{@alt}"/>
	</xsl:template>

</xsl:stylesheet>
