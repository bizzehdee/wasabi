<!-- Button Binding -->

<binding id="button" 
         minw="75" 
         minh="23"
         outlinecolor="black" 
         outlinesize="0" 
         wantfocus="1" 
         focusmargin="4,4,4,4" >

  <vbox flex="1" 
        margin="1,1,1,1" 
        bgstyle="raised" 
        outlinecolor="sys-3dshadow" 
        highlightcolor="sys-3dhighlight" 
        shadowcolor="sys-3ddarkshadow" 
        mousethrough="1">
    <vbox flex="1" 
          margin="1,1,1,1" 
          bgstyle="raised" 
          highlightcolor="sys-3dface">
      <spacer flex="1"/>
      <hbox> 
        <spacer flex="1"/>
        <spacer w="10"/>
        <vbox>
          <vbox id="content" 
                x="0" 
                y="0">

            <vbox flex="*">
              <spacer flex="1"/>
              <hbox>
                <spacer flex="1"/>
                <box inherit="image=btnimage,disabled" 
                     disabledimagealpha="48"/>
                <spacer flex="1"/>
              </hbox>
              <spacer flex="1"/>
            </vbox>

            <vbox flex="*">
              <spacer flex="1"/>
              <hbox>
                <spacer flex="1"/>
                <label y="0"
                       id="text" 
                       inherit="value=label,disabled" 
                       margin="0,0,0,1"
                       color="sys-buttontext" 
                       disabledcolor="sys-graytext,sys-highlighttext" />
                <spacer flex="1"/>
              </hbox>
              <spacer flex="1"/>
            </vbox>


          </vbox>
        </vbox>
        <spacer w="10"/>
        <spacer flex="1"/>
      </hbox>
      <spacer flex="1"/>
    </vbox>
  </vbox>
  
  <script>
    <![CDATA[

    var border1 = binding.enumChild(0);
    var border2 = border1.enumChild(0);
    var content = binding.getElementById("content");
    var text = binding.getElementById("text");
    var lbDownInButton = false;

    function bgStyle(style) {
      border1.setAttribute("bgstyle", style);
      border2.setAttribute("bgstyle", style);
    }

    function down() {
      bgStyle("flat");
      border1.setAttribute("outlinesize", "1");
      content.setAttribute("x", "1");
      content.setAttribute("y", "1");
    }

    function up(doevent) {
      bgStyle("raised");
      border1.setAttribute("outlinesize", "0");
      content.setAttribute("x", "0");
      content.setAttribute("y", "0");
      if (doevent)
        binding.triggerEvent("onbutton");
    }

    function onMouseDown() {
      lbDownInButton = true;
      down();
      binding.beginMouseCapture();
    }

    function onMouseUp() {
      binding.endMouseCapture();
      lbDownInButton = false;
      up(binding.isFocused() && 
         binding.isMouseIn());
    }

    function onCancelCapture() {
      lbDownInButton = false;
      up();
    }
  
    function onMouseEnter() {
      if (lbDownInButton)
        up();
    }

    function onMouseLeave() {
      if (lbDownInButton)
        down();
    }

    function setDefault(element, isdefault) {
      element.setAttribute("outlinesize", isdefault ? 1 : 0);
    }

    function onGetFocus() {
      if (window.defaultButton)
        setDefault(window.defaultButton, false);
      setDefault(binding, true);
    }

    function onLoseFocus() {
      setDefault(binding, false);
      if (window.defaultButton)
        setDefault(window.defaultButton, true);
      lbDownInButton = false;
      up(false);
    }

    function onChar(c) {
      if (c == 32 || 
          c == 13) {
        down();
        setTimeout(up, 100, true);
      }
    }

    binding.addEventListener("onleftbuttondown", onMouseDown);
    binding.addEventListener("onleftbuttonup", onMouseUp);
    binding.addEventListener("onmouseleave", onMouseEnter);
    binding.addEventListener("onmouseenter", onMouseLeave);
    binding.addEventListener("oncancelcapture", onCancelCapture);
    binding.addEventListener("ongetfocus", onGetFocus);
    binding.addEventListener("onlosefocus", onLoseFocus);
    binding.addEventListener("onchar", onChar);

    if (binding.getAttributeBool("default")) {
      window.defaultButton = binding;
      setDefault(binding, true);
    }

    ]]>

  </script>
  
</binding>
