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
window.alert = function(message) {
    // Check if the alert dialog already exists
    var alertDiv = document.getElementById('customAlert');
    if (!alertDiv) {
        // Create the alert dialog
        alertDiv = document.createElement('div');
        alertDiv.id = 'customAlert';
        alertDiv.className = 'custom-alert hidden';

        var contentDiv = document.createElement('div');
        contentDiv.className = 'custom-alert-content';

        var alertMessage = document.createElement('p');
        alertMessage.id = 'alertMessage';

        var okButton = document.createElement('button');
        okButton.id = 'alertOkButton';
        okButton.textContent = 'OK';
        okButton.onclick = function() {
            alertDiv.classList.add('hidden');
        };

        contentDiv.appendChild(alertMessage);
        contentDiv.appendChild(okButton);
        alertDiv.appendChild(contentDiv);
        document.body.appendChild(alertDiv);

        // Inject CSS
        var style = document.createElement('style');
        style.innerHTML = `
        .custom-alert {
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
        }
        .hidden {
            display: none;
        }`;
        document.head.appendChild(style);
    }

    // Show the alert dialog
    document.getElementById('alertMessage').textContent = message;
    alertDiv.classList.remove('hidden');
} 

// alert("Hello world!")
document.body.style.visibility = "visible"
