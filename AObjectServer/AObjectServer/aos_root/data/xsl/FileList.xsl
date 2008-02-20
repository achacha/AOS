<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format">
	<xsl:output method="html"/>
	<xsl:template match="/root">
		<xsl:apply-templates select="file-list"/>
	</xsl:template>
	<xsl:template match="file-list">
		<html>
			<head>
				<title>
				</title>
			</head>
			<body>
				<xsl:apply-templates select="file"/>
			</body>
		</html>
	</xsl:template>
	<xsl:template match="file">
		<xsl:value-of select="filename"/>
		<xsl:text>   </xsl:text>
		<br/>
	</xsl:template>
</xsl:stylesheet>
