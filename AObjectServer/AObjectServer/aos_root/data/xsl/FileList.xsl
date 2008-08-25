<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format">
  <xsl:output method="html"/>
  <xsl:template match="/root">
    <xsl:apply-templates select="file-list"/>
  </xsl:template>
  <xsl:template match="file-list">
    <xsl:variable name="relative-url">
      <xsl:value-of select="/root/REQUEST/url/path"/>
      <xsl:value-of select="/root/REQUEST/url/filename"/>
    </xsl:variable>
    <xsl:variable name="up" select="up"/>
    <html>
      <head>
        <title><xsl:value-of select="offset"/></title>
        <style type="text/css">
        a:link, a:visited {color:black; text-decoration:none}
        a:hover {color:rgb(192,7,15); background-color:rgb(200,200,200); text-decoration:none}
        img {border:0px; -moz-outline:0px}
        </style>
      </head>
      <body>
        <xsl:if test="$up != ''">
          <a href="{$relative-url}?offset={$up}">
            <img alt="^" src="/images/folder_open.gif"/>
            <xsl:text> ..</xsl:text>
          </a><br/>
        </xsl:if>
        <xsl:apply-templates select="file"/>
        <br/>
        <i><small>Request time: <xsl:value-of select="/root/request_time/interval"/>ms</small></i>
      </body>
    </html>
  </xsl:template>
  <xsl:template match="file">
	<xsl:if test="not(@hidden = 'true')">
		<xsl:variable name="relative-url">
		  <xsl:value-of select="/root/REQUEST/url/path"/>
		  <xsl:value-of select="/root/REQUEST/url/filename"/>
		</xsl:variable>
		<xsl:variable name="offset">
		  <xsl:value-of select="/root/file-list/offset"/>
		  <xsl:value-of select="filename/name"/>
		</xsl:variable>
		<xsl:choose>
		  <xsl:when test="@dir = '1'">
			<xsl:text> </xsl:text>
			<a href="{$relative-url}?offset={$offset}">
			  <img alt="+" src="/images/folder_shut.gif"/>
			  <xsl:text> </xsl:text>
			  <b><xsl:value-of select="filename/name"/></b>
			</a>
			<br/>
		  </xsl:when>
		  <xsl:otherwise>
			<xsl:text>  </xsl:text>
			<img alt="." src="/images/file.gif"/>
			<xsl:text> </xsl:text>
			<xsl:value-of select="filename/name"/>
			<br/>
		  </xsl:otherwise>
		</xsl:choose>
	</xsl:if>
  </xsl:template>
</xsl:stylesheet>
