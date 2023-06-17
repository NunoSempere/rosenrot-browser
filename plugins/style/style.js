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


// Replace default alert with new function
// whose style can be changed!
window.alert = (message) => {
    let alertDiv = document.getElementById('customAlert');
    if (!alertDiv) {
        const html = `
            <div id="customAlert" class="custom-alert">
                <div class="custom-alert-content">
                    <p id="alertMessage"></p>
                    <button id="alertOkButton">OK</button>
                </div>
            </div>
            <style>
                .custom-alert {
                    display: none;
                    position: fixed;
                    z-index: 999;
                    left: 0;
                    top: 0;
                    width: 100%;
                    height: 100%;
                    overflow: auto;
                    background-color: rgba(0,0,0,0.4);
                }
                .custom-alert-content {
                    background-color: #fefefe;
                    margin: 15% auto;
                    padding: 20px;
                    border: 1px solid #888;
                    width: 80%;
                    font-family: monospace; /* Use monospace font */
                }
                .visible {
                    display: block;
                }
            </style>
        `;
        document.body.insertAdjacentHTML('beforeend', html);
        alertDiv = document.getElementById('customAlert');
        document.getElementById('alertOkButton').onclick = () => {
            alertDiv.classList.remove('visible');
            document.removeEventListener('keydown', dismissAlert);
        };
    }
  
    const dismissAlert = (event) => {
        if (event.key === 'Enter' /*&& event.ctrlKey*/ && alertDiv.classList.contains('visible')) {
            alertDiv.classList.remove('visible');
            document.removeEventListener('keydown', dismissAlert);
        }
    }
  
    document.addEventListener('keydown', dismissAlert);
    document.getElementById('alertMessage').textContent = message;
    alertDiv.classList.add('visible');
}
// ^ takes 0.014ms to run, so performance is not the concern here.
// timed with console.time, console.timeEnd

document.body.style.visibility = "visible"
