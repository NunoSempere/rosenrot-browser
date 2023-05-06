// Replicates the Stylus app: <https://addons.mozilla.org/en-GB/firefox/addon/styl-us/>

var styles = null;

if (document.domain == "forum.effectivealtruism.org"){
	styles = `
	    .Layout-main {
                margin-left: 100px;
	    }
            .SingleColumnSection-root {
                width: 1000px !important;
                max-width: 1400px !important;
                padding-left: 100px !important;
            }
            .NavigationStandalone-sidebar {
                display: none;
            }
            .intercom-lightweight-app{
                display: none;
            }
  `
	  var styleSheet = document.createElement('style')
	  styleSheet.innerText = styles
	  document.head.appendChild(styleSheet)
	  console.log('Style changed')
}

if (document.domain == "mail.proton.me" ){
	styles = `
			.item-container-row.read, .item-container.read {
					background-color: white;
			}
			.item-container-row.unread, .item-container.unread {
					background-color: #E8E8E8;
			}
			.selection .item-container-row.item-is-selected, .item-container.item-is-selected {
					background-color: var(--selection-background-color) !important;
			}
	`
}
if (document.domain == "forum.nunosempere.com" ){
  styles = `
   body {
     zoom: 0.625 !important;
	 }
  `
}
if (document.domain == "search.nunosempere.com" ){
  styles = `
   footer {
     display: none;
	 }
  `
}
if (document.domain == "twitter.com" ){
  styles = `
	[data-testid="placementTracking"] {
		display: none;
	}
	[data-testid="sidebarColumn"] {
		display: none;
	}
  `
}

if(styles != null){
	  var styleSheet = document.createElement('style')
	  styleSheet.innerText = styles
	  document.head.appendChild(styleSheet)
	  console.log('Style changed')
}

document.body.style.visibility = "visible"

