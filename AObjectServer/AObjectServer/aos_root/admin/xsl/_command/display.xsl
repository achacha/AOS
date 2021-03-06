<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format">
	<xsl:output method="html"/>
	<xsl:template match="/">
		<html>
			<head>
				<title><xsl:value-of select="/admin/object/@name"/></title>
			</head>
			<body>
				<xsl:variable name="object_name" select="/admin/object/@name"/>
				<h1><xsl:value-of select="$object_name"/></h1>
				<a href="/admin?command=display&amp;object={$object_name}">Refresh <xsl:value-of select="$object_name"/></a> | 
				<a href="/admin?command=display&amp;object={$object_name}&amp;xml=1">Show <xsl:value-of select="$object_name"/> XML</a> | 
				<a href="/admin?command=list">Object list</a> | 
				<a href="/">Home</a><br/>
				<hr/>
				<br/>
				<table border="1">
					<tbody>
						<xsl:apply-templates select="/admin/object"/>
					</tbody>
				</table>
			</body>
		</html>
	</xsl:template>

	<xsl:template match="object">
		<xsl:param name="obj_parent_name"/>
		<xsl:variable name="obj_this_name" select="@name"/>
		<xsl:variable name="obj_this_index" select="@index"/>
		<xsl:variable name="obj_full_name">
			<xsl:choose>
				<xsl:when test="$obj_parent_name != ''">
					<xsl:choose>
						<xsl:when test="$obj_parent_name != ''">
							<xsl:choose>
								<xsl:when test="$obj_this_index != ''">
									<xsl:value-of select="concat($obj_parent_name, '.', $obj_this_name, '[', $obj_this_index, ']')"/>
								</xsl:when>
								<xsl:otherwise>
									<xsl:value-of select="concat($obj_parent_name, '.', $obj_this_name)"/>
								</xsl:otherwise>
							</xsl:choose>
						</xsl:when>
						<xsl:otherwise>
							<xsl:choose>
								<xsl:when test="$obj_this_index != ''">
									<xsl:value-of select="concat($obj_this_name, '[', $obj_this_index, ']')"/>
								</xsl:when>
								<xsl:otherwise>
									<xsl:value-of select="$obj_this_name"/>
								</xsl:otherwise>
							</xsl:choose>
						</xsl:otherwise>
					</xsl:choose>
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="$obj_this_name"/>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>

		<xsl:apply-templates select="error"/>
		<xsl:apply-templates select="message"/>

		<tr>
			<th bgcolor="#f0f0ff">
				<xsl:choose>
					<xsl:when test="@display">
						<xsl:value-of select="@display"/>
					</xsl:when>
					<xsl:otherwise>
						<xsl:value-of select="$obj_full_name"/>
					</xsl:otherwise>
				</xsl:choose>
				
			</th>
			<td>
				<table border="1" width="100%">
					<tbody>
						<xsl:apply-templates select="property">
							<xsl:with-param name="obj_parent_name" select="$obj_full_name"/>
							<xsl:with-param name="display_name" select="@display"/>
						</xsl:apply-templates>
					</tbody>
				</table>
			</td>
		</tr>

		<xsl:apply-templates select="object">
			<xsl:with-param name="obj_parent_name" select="$obj_full_name"/>
		</xsl:apply-templates>
		<xsl:apply-templates select="debugDump"/>
	</xsl:template>

	<xsl:template match="error">
		<font size="+2" color="red"><b>ERROR:</b> <xsl:value-of select="."/></font><br/>
	</xsl:template>

	<xsl:template match="message">
		<font color="green"><b>MESSAGE:</b> <xsl:value-of select="."/></font><br/>
	</xsl:template>


	<xsl:template match="property">
		<xsl:param name="obj_parent_name"/>
		<xsl:param name="display_name"/>
		<xsl:variable name="obj_this_name" select="@name"/>
		<xsl:variable name="obj_full_name">
			<xsl:choose>
				<xsl:when test="$obj_parent_name != ''">
					<xsl:value-of select="concat($obj_parent_name, '.', $obj_this_name)"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="$obj_this_name"/>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		<tr>
			<th align="left" bgcolor="#dfdfdf">
				<xsl:value-of select="@name"/>
			</th>
			<xsl:choose>
				<xsl:when test="action">
					<td bgcolor="#d0ffd0" align="center"><xsl:value-of select="value"/></td>
					<td bgcolor="#d0ffd0" align="left" valign="center">
						<form action="/admin" method="get">
						<input type="hidden" name="display_name" value="{$display_name}"/>
						<xsl:apply-templates select="action">
							<xsl:with-param name="obj_full_name" select="$obj_full_name"/>
 							<xsl:with-param name="action_oid" select="parent::node()/@oid"/>
 						</xsl:apply-templates>
						</form>
					</td>
				</xsl:when>
				<xsl:otherwise>
					<td colspan="2" valign="middle"><pre><xsl:value-of select="value"/></pre></td>
				</xsl:otherwise>
			</xsl:choose>
		</tr>
	</xsl:template>

	<xsl:template match="action">
		<xsl:param name="obj_full_name"/>
		<xsl:param name="display_name"/>
		<xsl:param name="action_oid"/>
		<xsl:variable name="action_name" select="name()"/>
		<xsl:variable name="action_index" select="@index"/>
		<xsl:variable name="object_name" select="/admin/object/@name"/>
			<input type="hidden" name="command" value="modify"/>
			<input type="hidden" name="object" value="{$object_name}"/>
			<input type="hidden" name="property" value="{$obj_full_name}"/>
			<input type="hidden" name="index" value="{$action_index}"/>
			<xsl:for-each select="param">
				<xsl:variable name="input_field" select="."/><xsl:value-of select="$input_field"/><input name="{$input_field}"/><br/>
			</xsl:for-each>
			<xsl:choose>
				<xsl:when test="$action_oid != ''">
					<input type="hidden" name="oid" value="{$action_oid}"/>
					<input type="submit" name="action" value="{@name} {$action_oid}"/>
				</xsl:when>
				<xsl:otherwise>
					<input type="submit" name="action" value="{@name}"/>
				</xsl:otherwise>
			</xsl:choose>
			<br/><i><pre><xsl:value-of select="@desc"/></pre></i><br/>
	</xsl:template>

	<xsl:template match="debugDump">
		<tr>
			<th align="left" colspan="3" bgcolor="#ddddff">
				<pre>
					<xsl:value-of select="."/>
				</pre>
			</th>
		</tr>
	</xsl:template>

</xsl:stylesheet>
