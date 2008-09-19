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
			<b>Request execution time: <xsl:value-of select="request_time"/></b>
			<br/>
			<b>Context execution time: <xsl:value-of select="context_time"/></b>
			</body>
		</html>
	</xsl:template>
	
	<xsl:template match="execute">
	<table border="2">
		<tbody>
			<tr><th>Input Processor</th><td><xsl:value-of select="input"/></td></tr>
			<tr><th>Module(s)</th><td>
        <xsl:for-each select="module">
					<xsl:value-of select="."/><br/>
				</xsl:for-each>
      </td></tr>
			<tr><th>Output Generator</th><td><xsl:value-of select="output"/></td></tr>
		</tbody>
	</table>
	</xsl:template>
</xsl:stylesheet>
