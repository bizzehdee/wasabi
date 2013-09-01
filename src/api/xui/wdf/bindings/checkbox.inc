<!-- CheckBox Binding -->

<binding id="checkbox" 
         wantfocus="1" 
         focusmargin="16,0,0,0">

  <vbox flex="1" mousethrough="1">
    <spacer flex="1"/>
    <hbox>
      <vbox>
        <spacer flex="1"/>
        <box margin="1,1,1,1" 
             bgstyle="sunken" >
          <box id="content" 
               margin="1,1,1,1" 
               bgcolor="white" 
               bgstyle="sunken" 
               highlightcolor="sys-3dface" 
               shadowcolor="sys-3ddarkshadow">
            <box id="check" 
                 margin="1,1,1,1" 
                 image="checkbox-on" 
                 disabledimagealpha="128" 
                 inherit="disabled" />
          </box>
        </box>
        <spacer flex="1"/>
      </vbox>
      <vbox> 
        <spacer flex="1"/>
        <label id="label" 
               inherit="value=label,disabled" />
        <spacer flex="1"/>
      </vbox>
    </hbox>
    <spacer flex="1"/>
  </vbox>

  <script>

    function updateImages() {
      var ischecked = binding.getAttributeBool("checked");
      var check = binding.getElementById("check");
      check.setAttribute("image", "checkbox-" + (ischecked ? "on" : "off"));
      var isdisabled = binding.getAttributeBool("disabled");
      var contentbox = binding.getElementById("content");
      contentbox.setAttribute("bgcolor", isdisabled ? "sys-3dface" : "white");
    }

    function toggle() {
      if (binding.getAttributeBool("disabled")) return;
      var checked = !binding.getAttributeBool("checked");
      binding.setAttribute("checked", checked);
      binding.triggerEvent("ontoggle", checked);
    }

    binding.addEventListener("onattributechanged", 
      function(attr, newvalue) {
        if (attr == "checked")
          updateImages();
      }
    );

    binding.addEventListener("onleftbuttondown", toggle);
    binding.addEventListener("onchar", function(c) { if (c == 32) toggle(); });

    updateImages();

  </script>

</binding>
