<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format">
	<xsl:template match="/root">
		<html>
			<head>
				<title>Simple AOS xslt example</title>
			</head>
			<body>
			<h1>Simple AOS xslt example</h1>
			<xsl:apply-templates select="execute"/>
			<br/>
			<b>Total execution time: <xsl:value-of select="total_time"/></b>
			</body>
		</html>
	</xsl:template>
	
	<xsl:template match="execute">
	<table border="2">
		<tbody>
			<tr><th>Input Processor</th><td><xsl:value-of select="input"/></td></tr>
				<xsl:for-each select="module">
					<tr><th>Module</th><td><xsl:value-of select="."/></td></tr>
				</xsl:for-each>
			<tr><th>Output Generator</th><td><xsl:value-of select="output"/></td></tr>
		</tbody>
	</table>
	</xsl:template>
</xsl:stylesheet>
