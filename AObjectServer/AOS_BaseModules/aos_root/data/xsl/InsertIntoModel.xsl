<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format">
	
	<xsl:template  match="/">
		<html>
			<head>
				<title>Insert Into Model</title>
			</head>
			<body>
				<h2>Content found under /root/data/insert-root-2/item</h2>
				<xsl:apply-templates select="root/data/insert-root-2/item"/><br/>
			</body>
		</html>
	</xsl:template>
	
	<xsl:template match="item">
		<b><xsl:value-of select="." /></b><br/>
	</xsl:template>
</xsl:stylesheet>
