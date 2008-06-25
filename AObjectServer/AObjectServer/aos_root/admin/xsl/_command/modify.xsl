<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format">
  <xsl:output method="html"/>
  <xsl:template match="/">
    <xsl:variable name="redirectUrl">
      <xsl:text>/admin?command=display&amp;object=</xsl:text>
      <xsl:value-of select="/admin/object/@name"/>
    </xsl:variable>
    
    <html>
      <head>
        <title>Modify redirect page</title>
        <xsl:element name="meta">
          <xsl:attribute name="http-equiv">Refresh</xsl:attribute>
          <xsl:attribute name="content">
            <xsl:text>0;</xsl:text>
            <xsl:value-of select="$redirectUrl"/>
          </xsl:attribute>
        </xsl:element>
      </head>
      <body>
        <a href="/admin?command=list">Object list</a> |
        <a href="/">Home</a><br/>
        <hr/>
        <br/>

        <xsl:element name="a">
          <xsl:attribute name="href">
            <xsl:value-of select="$redirectUrl"/>
          </xsl:attribute>
          <xsl:text>Click if redirect fails</xsl:text>
        </xsl:element>
        
      </body>
    </html>
  </xsl:template>
</xsl:stylesheet>
