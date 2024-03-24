// Inspired by the Stylus app: <https://addons.mozilla.org/en-GB/firefox/addon/styl-us/>

// Main part of the code: switch on the domain and select the corresponding style
var styles = null;
console.log(document.domain)
switch (document.domain) {
  case "forum.effectivealtruism.org":
    styles = `
    /*
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
    */
    `;
    break;
  case "nationstates.net":
    styles = `
      .adidentifier {
        display: none;
      }
    `;
    break;
  case "mail.proton.me":
    styles = `
	  /*
      .item-container-row.read, .item-container.read {
		      background-color: white;
      }
      .item-container-row.unread, .item-container.unread {
		      background-color: #E8E8E8;
      }
      .selection .item-container-row.item-is-selected, .item-container.item-is-selected {
		      background-color: var(--selection-background-color) !important;
      }
      zoom: 0.625 !important;
	  */
	  `;
    break;
  case "forum.nunosempere.com":
    styles = `
      body {
       zoom: 0.625 !important;
      }
    `;
    break;
  case "search.brave.com":
    styles = `
      .download-button,
      a[href^="https://brave.com/download/"] {
        display: none !important; 
      }
    `;
    break;
  case "search.nunosempere.com":
    styles = `
    /* 
      body {
        zoom: 1.8;
      }
	  */

      footer {
        display: none;
      }
    `;
    break;
  case "reddit.com":
  // fallthrough
  case "old.reddit.com":
    styles = `
	    /* kill sidebar ads */
	    .ad-container,
	    a[href^="https://alb.reddit.com"]
	    a[href="/premium"],
	    [data-promoted^="true"],
	    #eu-cookie-policy,
	    .infobar-toaster-container,
	    .listingsignupbar,
	    .native-ad-container,
	    .native-sidebar-ad,
	    .premium-banner-outer,
	    .promotedlink,
	    .promoted
	    {
		    display: none !important;
	    }
	  `;
    break;
  case "twitter.com":
    styles = `
	    /* hide promoted tweets */
	    :has(meta[property="og:site_name"][content="Twitter"])
		    [data-testid="cellInnerDiv"]:has(svg + [dir="auto"]) {
		    display: none;
	    }
	    [data-testid^="placementTracking"] {
		    display: none;
	    }

      /* hide what's happening section */
      :has(meta[property="og:site_name"][content="Twitter"])
        [aria-label="Timeline: Trending now"] {
        display: none !important;
      }
	    [data-testid^="sidebarColumn"] {
		    display: none;
	    }

	    /* Hide DMs v2 */
	    [data-testid^="DMDrawerHeader"] {
		    display: none;
	    }

	    /* Tweak main column */
	    [data-testid^="primaryColumn"] {
		    min-width: 900px;
		    max-width: 900px;
	    }
	    [data-testid^="cellInnerDiv"] {
		    min-width: 700px;
		    max-width: 700px;
	    }
	    [aria-label^="Timeline: Conversation"]{
	      margin-left: 145px;
	    }
	    [data-testid^="DMDrawer"]{
	      display: none;
	    }

	    /* Delete a few unused or annoying elements */
	    [aria-label^="Verified Orgs"] {
		    display: none;
	    }
	    [aria-label^="Lists"] {
		    display: none;
	    }
	    [aria-label^="Communities"] {
		    display: none;
	    }
	    [aria-label^="Primary"] {
		    margin-top: 50px;
	    }
	    [role^="progressbar"]{
		    display: none;
	    }

	    /* hide video */
      [data-testid^="videoPlayer"] {
        display: none !important;
      }

      /* No change of colors in hover */
      *:hover {
        /* background-color: white !important; */
        transition: none !important;
      }*/
      /*
      *:hover {
        background-color: inherit !important;
        transition: none !important;
      }*/
      /* Hide go to top button */
      [aria-label^="New posts are available. Push the period key to go to the them."]{
		    display: none;
      }

      /* No transparency at the top */
      [aria-live^="polite"]{
        background: white !important;
      }
    `;
    break;
  default:
    console.log("No custom style");
}

if (styles != null) {
  var styleSheet = document.createElement("style");
  styleSheet.innerText = styles;
  document.head.appendChild(styleSheet);
  console.log("Style changed");
}

// Extra: Replace default alert with new function
// whose style can be changed!
window.alert = (message) => {
  let alertDiv = document.getElementById("customAlert");
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
    document.body.insertAdjacentHTML("beforeend", html);
    alertDiv = document.getElementById("customAlert");
    document.getElementById("alertOkButton").onclick = () => {
      alertDiv.classList.remove("visible");
      document.removeEventListener("keydown", dismissAlert);
    };
  }

  const dismissAlert = (event) => {
    if (
      event.key === "Enter" /*&& event.ctrlKey*/ &&
      alertDiv.classList.contains("visible")
    ) {
      alertDiv.classList.remove("visible");
      document.removeEventListener("keydown", dismissAlert);
    }
  };

  document.addEventListener("keydown", dismissAlert);
  document.getElementById("alertMessage").textContent = message;
  alertDiv.classList.add("visible");
};

// Extra: hide video players on twitter
if (document.domain == "twitter.com") {
  // Function to hide the grandparent of video players
  // takes 0.014ms to run, so performance is not the concern here.
  // timed with console.time, console.timeEnd

  function hideVideoPlayerGrandparent() {
    document
      .querySelectorAll('[data-testid="videoPlayer"]')
      .forEach(function (videoPlayer) {
        var grandparentElement =
          videoPlayer.parentElement.parentElement.parentElement.parentElement
            .parentElement.parentElement;
        var newTextElement = document.createElement("div");
        newTextElement.textContent = " [ twitter video ] ";
        newTextElement.style["margin-top"] = "10px";
        newTextElement.style["margin-left"] = "10px";
        newTextElement.style["margin-bottom"] = "10px";
        grandparentElement.replaceWith(newTextElement);
      });
  }

  // Create a new MutationObserver instance
  var observer = new MutationObserver(function (mutations) {
    mutations.forEach(function (mutation) {
      if (mutation.addedNodes.length) {
        hideVideoPlayerGrandparent(); // Call the function to hide video players
      }
    });
  });

  // Options for the observer (which mutations to observe)
  var config = { childList: true, subtree: true };

  // Start observing the target node for configured mutations
  observer.observe(document.body, config);

  // Call the function initially to hide any video players on initial load
  hideVideoPlayerGrandparent();
}

// document.body.style.visibility = "visible";
