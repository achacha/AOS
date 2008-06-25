<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format">
	<xsl:output method="html"/>
	<xsl:template match="/">
		<html>
			<head>
				<title>ADMIN: Objects</title>
			</head>
			<body>

        <a href="/admin?command=list">Object list</a> |
        <a href="/">Home</a><br/>
        <hr/>
        <br/>

        <table border="1" cellpadding="3" cellspacing="3">
					<tbody>
						<tr>
							<th>Object</th>
						</tr>
						<xsl:for-each select="admin/object">
							<xsl:variable name="obj" select="@name"/>
							<xsl:variable name="obj_url" select="concat('/admin?command=display&amp;object=', $obj)"/>
							<xsl:variable name="obj_url_debug" select="concat('/admin?command=display&amp;debugDump=1&amp;object=', $obj)"/>
							<xsl:variable name="obj_url_xml" select="concat('/admin?command=display&amp;debugDump=1&amp;xml=1&amp;object=', $obj)"/>
							<tr>
								<td bgcolor="#eeeeee">
									<a href="{$obj_url}"><xsl:value-of select="@name"/></a> <font size="1"> (<a href="{$obj_url_debug}">debugDump</a>)</font>
								</td>
								<td><small><a href="{$obj_url_xml}">xml</a></small></td>
							</tr>
						</xsl:for-each>
					</tbody>
				</table>
			</body>
		</html>
	</xsl:template>
</xsl:stylesheet>
