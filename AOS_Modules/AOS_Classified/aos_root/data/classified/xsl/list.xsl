<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format">
	<xsl:template match="/">
		<html>
			<head>
				<title>Classified item list</title>
			    <link href="/classified/classified.css" type="text/css" rel="stylesheet"/>
			</head>
			<body>
			<h1>List of classified items</h1>
			<br/>
			<table border="1" cellpadding="2" cellspacing="2" width="100%">
				<tbody>
					<tr>
						<th>Title</th>
						<th>Description</th>
						<th>Contact Email</th>
						<th>Price</th>
					</tr>
					<xsl:apply-templates select="/root/classified/row"/>
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
			<td><div class="cla_title"><xsl:value-of select="title"/></div></td>
			<td><div class="cla_description"><xsl:value-of disable-output-escaping="yes" select="descript"/></div></td>
			<td><div class="cla_contact"><xsl:value-of select="contact"/></div></td>
			<td><div class="cla_price"><xsl:value-of select="price"/></div></td>
		</tr>
	</xsl:template>
</xsl:stylesheet>
