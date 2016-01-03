
// require("js/omv/WorkspaceManager.js")

OMV.WorkspaceManager.registerNode({
	id: "networking",
	path: "/",
	text: _("Networking"),
	position: 15
});

OMV.WorkspaceManager.registerNodeOverride({
	newpath : "/networking",
    id      : "hostapd",
    path    : "/service",
//    text    : _("Hostapd"),
//    icon16  : "images/hostapd.png",
//    iconSvg : "images/hostapd.svg"
});

OMV.WorkspaceManager.registerPanelOverride({
	newpath   : "/networking/hostapd",
    id        : "settings",
    path      : "/service/hostapd",
//    text      : _("Settings"),
//    position  : 10,
//    className : "OMV.module.admin.service.hostapd.Settings"
});
