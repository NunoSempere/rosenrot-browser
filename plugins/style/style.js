// Replicates the Stylus app: <https://addons.mozilla.org/en-GB/firefox/addon/styl-us/>

if (document.domain == "forum.effectivealtruism.org"){
	var styles = `
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
	var styles = `
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
	  var styleSheet = document.createElement('style')
	  styleSheet.innerText = styles
	  document.head.appendChild(styleSheet)
	  console.log('Style changed')
}

document.body.style.visibility = "visible"
