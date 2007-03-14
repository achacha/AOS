<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format">
	<xsl:template match="/root">
		<html>
			<head>
				<title>Classified submit</title>
			</head>
			<body>
			<h1>Item submitted.</h1>
			<br/>
			<xsl:apply-templates select="error"/>
			<br/>
			<br/>
			<div align="center"><small>Total execution time: <xsl:value-of select="total_time"/></small></div>
			</body>
		</html>
	</xsl:template>

	<xsl:template match="error">
		<font color="red"><b>ERROR:</b> <xsl:value-of select="."/></font><br/>
	</xsl:template>
</xsl:stylesheet>
