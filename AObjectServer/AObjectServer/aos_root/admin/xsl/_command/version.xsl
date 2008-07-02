<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format">
	<xsl:output method="html"/>
	<xsl:template match="/">
		<html>
			<head>
				<title>ADMIN: Version</title>
			</head>
			<body>
				<h3>Version information</h3>
				<table border="1">
					<xsl:apply-templates select="/admin/version"/>
				</table>
			</body>
		</html>
	</xsl:template>
	<xsl:template match="version">
		<tr>
			<td>
				<xsl:value-of select="text()"/>
			</td>
		</tr>
	</xsl:template>
</xsl:stylesheet>
