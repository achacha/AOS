<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format">
	<xsl:output method="html"/>
	<xsl:template match="/">
		<html>
			<head>
				<title>ADMIN: Version</title>
			</head>
			<body>
        <h3>
          <xsl:value-of select="/admin/version"/>
        </h3>
			</body>
		</html>
	</xsl:template>
</xsl:stylesheet>
