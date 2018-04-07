<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="index.aspx.cs" Inherits="webTest.WebForm1" %>

<!DOCTYPE html>

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
   
<title>
Slice Demo	
</title>
</head>
<body>
    <form id="form1" runat="server">
    <h1>
气管
</h1>
<div id="openseadragon1" style="width: 1220px; height: 1000px;background:#000"></div>
<script type="text/javascript">
function pad(num, n) {
    return (Array(n).join(0) + num).slice(-n);
}
</script>
    <script src="http://ajax.googleapis.com/ajax/libs/jquery/1.8.0/jquery.min.js">
    </script>
<script src="/openseadragon/openseadragon.min.js"></script>
<script type="text/javascript">
    var viewer = OpenSeadragon({
    id:            "openseadragon1",
debugMode:true,
minZoomLevel:0.001,
maxZoomLevel:160,
defaultZoomLevel:0,
showNavigator:  true,
    tileSources:   {
	type: 'openstreetmaps',
	width:229724,
	height:96218,
	tileSize:256,
	getTileUrl: function (level, x, y) {
            return "Handler1.ashx?level="+level.toString()+"&x="+x.toString()+"&y="+y.toString();
        }
    }
});	
</script>

    </form>
</body>
</html>
