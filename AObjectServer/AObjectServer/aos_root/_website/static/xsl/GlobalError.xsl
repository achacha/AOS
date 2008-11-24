<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format">
	<xsl:output method="html"/>
	<xsl:template match="/">
		<html>
		<head><title>Async Object Serve Error</title></head>
		<body>
			<h2>HTTP status code 500: An internal server error has occured.</h2>
			<p>The following error events may be useful in determining the case of the error.</p>
			<table border="1">
				<tr>
					<th><xsl:value-of select="/root/error/Name/text()"/></th>
				</tr>
				<xsl:apply-templates select="/root/error/Events/Event"/>
			</table>
			<br/>
			<p>If the error persists please contact the administrator.</p>
		</body>
		</html>
	</xsl:template>
	
	<xsl:template match="Event">
		<xsl:if test="@error = 'true'">
			<tr><td bgcolor="#FFDDDD"><xsl:value-of select="."/></td></tr>
		</xsl:if>
	</xsl:template>
</xsl:stylesheet>