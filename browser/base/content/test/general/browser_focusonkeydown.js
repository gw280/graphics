add_task(function *() {
  let keyUps = 0;

  yield BrowserTestUtils.openNewForegroundTab(gBrowser, "data:text/html,<body>");

  gURLBar.focus();

  window.addEventListener("keyup", function(event) {
    if (event.originalTarget == gURLBar.inputField) {
      keyUps++;
    }
  }, {capture: true, once: true});

  gURLBar.addEventListener("keydown", function(event) {
    gBrowser.selectedBrowser.focus();
  }, {capture: true, once: true});

  EventUtils.synthesizeKey("v", { });

  is(keyUps, 1, "Key up fired at url bar");

  gBrowser.removeCurrentTab();
});
