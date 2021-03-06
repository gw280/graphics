function test() {
  waitForExplicitFinish();

  var tab = gBrowser.addTab();

  tab.addEventListener("TabClose", function() {
    ok(tab.linkedBrowser, "linkedBrowser should still exist during the TabClose event");

    executeSoon(function() {
      ok(!tab.linkedBrowser, "linkedBrowser should be gone after the TabClose event");

      finish();
    });
  }, {once: true});

  gBrowser.removeTab(tab);
}
