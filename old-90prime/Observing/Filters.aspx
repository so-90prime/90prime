<%@ Assembly Name="Microsoft.SharePoint, Version=14.0.0.0, Culture=neutral, PublicKeyToken=71e9bce111e9429c"%> <%@ Page Language="C#" Inherits="Microsoft.SharePoint.WebPartPages.WikiEditPage" MasterPageFile="~masterurl/default.master"       %> <%@ Import Namespace="Microsoft.SharePoint.WebPartPages" %> <%@ Register Tagprefix="SharePoint" Namespace="Microsoft.SharePoint.WebControls" Assembly="Microsoft.SharePoint, Version=14.0.0.0, Culture=neutral, PublicKeyToken=71e9bce111e9429c" %> <%@ Register Tagprefix="Utilities" Namespace="Microsoft.SharePoint.Utilities" Assembly="Microsoft.SharePoint, Version=14.0.0.0, Culture=neutral, PublicKeyToken=71e9bce111e9429c" %> <%@ Import Namespace="Microsoft.SharePoint" %> <%@ Assembly Name="Microsoft.Web.CommandUI, Version=14.0.0.0, Culture=neutral, PublicKeyToken=71e9bce111e9429c" %>
<%@ Register Tagprefix="WebPartPages" Namespace="Microsoft.SharePoint.WebPartPages" Assembly="Microsoft.SharePoint, Version=14.0.0.0, Culture=neutral, PublicKeyToken=71e9bce111e9429c" %>
<asp:Content ContentPlaceHolderId="PlaceHolderPageTitle" runat="server">
	<SharePoint:ProjectProperty Property="Title" runat="server"/> - <SharePoint:ListItemProperty runat="server"/>
</asp:Content>
<asp:Content ContentPlaceHolderId="PlaceHolderPageTitleInTitleArea" runat="server">
	<span>
		<SharePoint:DocumentFolderName runat="server" id="PageFolderName" AppendSeparatorArrow = "true" />
	</span>
	<span class="ms-WikiPageNameEditor-Display" id="wikiPageNameDisplay" runat="server">
		<SharePoint:ListItemProperty runat="server"/>
	</span>
	<span class="ms-WikiPageNameEditor-Edit" style="display:none;" id="wikiPageNameEdit" runat="server">
		<asp:TextBox id="wikiPageNameEditTextBox" runat="server" />
	</span>
</asp:Content>
<asp:Content ContentPlaceHolderId="PlaceHolderPageDescription" runat="server">
	<SharePoint:UIVersionedContent runat="server" UIVersion="4">
		<ContentTemplate>
			<SharePoint:ProjectProperty Property="Description" runat="server"/>
		</ContentTemplate>
	</SharePoint:UIVersionedContent>
</asp:Content>
<asp:Content ContentPlaceHolderId="PlaceHolderPageImage" runat="server"><SharePoint:AlphaImage ID=onetidtpweb1 Src="/_layouts/images/wiki.png" Width=145 Height=54 Alt="" Runat="server" /></asp:Content>
<asp:Content ContentPlaceHolderId="PlaceHolderAdditionalPageHead" runat="server">
	<meta name="CollaborationServer" content="SharePoint Team Web Site" />
	<script type="text/javascript">
	var navBarHelpOverrideKey = "WSSEndUser";
	</script>
	<SharePoint:RssLink runat="server" />
	<SharePoint:UIVersionedContent UIVersion="4" runat="server"><ContentTemplate>
		<SharePoint:CssRegistration runat="server" Name="wiki.css" />
	</ContentTemplate></SharePoint:UIVersionedContent>
</asp:Content>
<asp:Content ContentPlaceHolderId="PlaceHolderMiniConsole" runat="server">
	<SharePoint:FormComponent TemplateName="WikiMiniConsole" ControlMode="Display" runat="server" id="WikiMiniConsole"/>
</asp:Content>
<asp:Content ContentPlaceHolderId="PlaceHolderLeftActions" runat="server">
	<SharePoint:RecentChangesMenu runat="server" id="RecentChanges"/>
</asp:Content>
<asp:Content ContentPlaceHolderId="PlaceHolderMain" runat="server">
	<SharePoint:UIVersionedContent runat="server" UIVersion="3" Id="PlaceHolderWebDescription">
		<ContentTemplate>
			<div class="ms-webpartpagedescription"><SharePoint:ProjectProperty Property="Description" runat="server"/></div>
		</ContentTemplate>
	</SharePoint:UIVersionedContent>
	<asp:UpdatePanel
		   id="updatePanel"
		   runat="server"
		   UpdateMode="Conditional"
		   ChildrenAsTriggers="false">
		<ContentTemplate>
			<SharePoint:VersionedPlaceHolder UIVersion="4" runat="server">
				<SharePoint:SPRibbonButton
					id="btnWikiEdit"
					RibbonCommand="Ribbon.WikiPageTab.EditAndCheckout.SaveEdit.Menu.SaveEdit.Edit"
					runat="server"
				    Text="edit"/>
				<SharePoint:SPRibbonButton
					id="btnWikiSave"
					RibbonCommand="Ribbon.WikiPageTab.EditAndCheckout.SaveEdit.Menu.SaveEdit.SaveAndStop"
					runat="server"
				    Text="edit"/>
				<SharePoint:SPRibbonButton
					id="btnWikiRevert"
					RibbonCommand="Ribbon.WikiPageTab.EditAndCheckout.SaveEdit.Menu.SaveEdit.Revert"
				    runat="server"
					Text="Revert"/>
			</SharePoint:VersionedPlaceHolder>
			<SharePoint:EmbeddedFormField id="WikiField" FieldName="WikiField" ControlMode="Display" runat="server"/>
			<WebPartPages:WebPartZone runat="server" ID="Bottom" Title="loc:Bottom" />
	</ContentTemplate>
	<Triggers>
	    <asp:PostBackTrigger ControlID="WikiField" />
	    <asp:PostBackTrigger ControlID="btnWikiRevert" />
	    <asp:PostBackTrigger ControlID="btnWikiSave" />
	</Triggers>
 </asp:UpdatePanel>
</asp:Content>
