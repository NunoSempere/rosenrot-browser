(() => {
    'use strict';

    const addStyle = (() => {
        const parent = document.head || document.body || document.documentElement;

        const style = document.createElement('style');
        style.type = 'text/css';
        parent.appendChild(style);

        return (css) => {
            style.appendChild(document.createTextNode(css + '\n'));
        };
    })();

    Array.from(document.styleSheets, (css) => css.disabled = true);

    // like a Firefox Reader View
    const nightmode = `
        * {
            margin-top: initial !important;
            padding-top: initial !important;
        }
        body {
            margin-left: auto !important;
            margin-right: auto !important;
            max-width: 70% !important;
            font-size: 1.6em !important;
            line-height: 1.25em !important;
            background-color: #343A3A !important;
            color: #FDFDFD !important;
            white-space: pre-line !important;
        }
        h1 {
            line-height: 1em !important;
        }
        a {
            color: lightskyblue !important;
            background-color: initial !important;
        }
        img {
            max-width: 100% !important;
        }
        pre, code {
            white-space: pre-wrap !important;
        }
    `;

    addStyle(nightmode);
})();
