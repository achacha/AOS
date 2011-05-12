<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="html" indent="yes"/>
  <xsl:template match="/root">
    <html>
      <head>
        <title>Global variables</title>
        <script type="text/javascript" src="/js/jquery/jquery-1.6.js"></script>
        <script type="text/javascript" src="/js/jquery/jquery.colorize-1.2.0.js"></script>

        <script type="text/javascript">
<![CDATA[
$("#g0").colorize();
]]>
</script>
      </head>
      <body>
        <span style="font-size:16pt">Results from <span style="font-weight:300; color:red">select * from global</span></span>
        <br/>
        <hr/>
        <br/>
        <table id="g0">
          <tr>
            <th width="150">Name</th>
            <th width="250">Value</th>
          </tr>  
          <xsl:apply-templates select="row"/>
        </table>
        <br/>
      </body>
    </html>
  </xsl:template>

  <xsl:template match="row">
    <tr>
      <td width="150"><xsl:value-of select="name"/></td>
      <td width="250"><xsl:value-of select="value"/></td>
    </tr>
  </xsl:template>

</xsl:stylesheet>
