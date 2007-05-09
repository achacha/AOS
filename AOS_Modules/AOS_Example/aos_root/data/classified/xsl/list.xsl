<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format">
	<xsl:template match="/">
		<html>
			<head>
				<title>Classified list</title>
			</head>
			<body>
			<h1>List of classified items</h1>
			<br/>
			<table border="1" cellpadding="1" cellspacing="1">
				<tbody>
					<tr>
						<th>Title</th>
						<th>Description</th>
						<th>Contact Email</th>
						<th>Price</th>
					</tr>
					<xsl:apply-templates select="/root/classified/AResultSet/row"/>
				</tbody>
			</table>
			<br/>
			<br/>
			<div align="center"><small>Request execution time: <xsl:value-of select="/root/request_time"/></small></div>
			</body>
		</html>
	</xsl:template>

	<xsl:template match="row">
		<tr>
			<td><div><xsl:value-of select="title"/></div></td>
			<td><div><xsl:value-of select="descript"/></div></td>
			<td><div><xsl:value-of select="contact"/></div></td>
			<td><div><xsl:value-of select="price"/></div></td>
		</tr>
	</xsl:template>
</xsl:stylesheet>
