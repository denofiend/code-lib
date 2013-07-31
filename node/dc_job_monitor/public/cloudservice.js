Ext.require([
  'Ext.window.MessageBox'
]);

Ext.application({
  name: 'CloudService',
  launch: function() {
    function datetime_renderer(value){
      var d = new Date(value*1000);
      return ''+d.getFullYear()+'-'+(d.getMonth()+1)+'-'+d.getDate()+' '+d.toLocaleTimeString();
    };

    var WIDTH = 1000;
    var HEIGHT = 600;

    function get_user(){
      var user = 0;
      try{
        user = parseInt(userText.getValue());
      }catch(error){
        return false;
      }
      if(user<=0){
        return false;
      }
      return user;
    }

    function computersize_renderer(value){
      if(value>1000000000){
        return (value/1000000000).toFixed(2)+'G';
      }else if(value>1000000){
        return (value/1000000).toFixed(2)+'M';
      }else if(value>1000){
        return (value/1000).toFixed(2)+'K';
      }
      return value+'B';
    };

    function deviceid_renderer(value){
      return value.toUpperCase();
    };

    var apps = Ext.create('Ext.data.Store', {
      fields: ['app', 'label'],
      data : [
        {"app":"tbsc", "label":"云标签"},
        {"app":"disk", "label":"云下载"},
        {"app":"dsnd", "label":"云传送"}
      ]
    });

    Ext.define('MxDevice',{
      extend: 'Ext.data.Model',
      fields: [
        'online',
        'device_id',
        'device_type',
        'bind_time',
        'device_name',
        'device_ip',
        'device_resolution',
        'language',
        'last_login_time',
        'os_version',
        'login_count']
    });
    Ext.define('MxFile',{
      extend: 'Ext.data.Model',
      fields:[
        'path',
        'mt',
        'size']
    });
    Ext.define('MxMessage',{
      extend: 'Ext.data.Model',
      fields:[
        'from_device',
        'to_device',
        'screen_name',
        'type',
        'path']
    });

    var deviceAjax = new Ext.data.proxy.Ajax({
      model: 'MxDevice',
      type: 'ajax',
      reader: {
        type: 'json', root: 'items', totalProperty: 'total'
      }
    });

    var deviceStore = Ext.create('Ext.data.Store',{
      model: 'MxDevice',
      autoLoad: false,
      pageSize: 10,
      proxy: deviceAjax
    });

    var fileAjax = new Ext.data.proxy.Ajax({
      model: 'MxFile',
      type: 'ajax',
      reader: {
        type: 'json', root: 'items', totalProperty: 'total'
      }
    });

    var fileStore = Ext.create('Ext.data.Store',{
      model: 'MxFile',
      autoLoad: false,
      pageSize: 10,
      proxy: fileAjax
    });

    var messageAjax = new Ext.data.proxy.Ajax({
      type: 'ajax',
      reader: {
        type: 'json', root: 'items', totalProperty: 'total'
      }
    });

    var messageStore = Ext.create('Ext.data.Store',{
      model: 'MxMessage',
      autoLoad: false,
      pageSize: 10,
      proxy: messageAjax
    });

    var devicePanel = Ext.create('Ext.grid.Panel',{
      store: deviceStore,
      title: '设备',
      columns: [{
        text:'在线',
        width:50,
        dataIndex:'online',
        renderer:function(value){return value?'√':'×'}
      },{
        text:'设备标识',
        width:300,
        dataIndex:'device_id',
        renderer:deviceid_renderer
      },{
        text:'设备类型',
        width:100,
        dataIndex:'device_type'
      },{
        text:'绑定时间',
        width:150,
        dataIndex:'bind_time',
        renderer: datetime_renderer
      },{
        text:'设备名称',
        width:150,
        dataIndex:'device_name'
      },{
        text:'IP地址',
        width:100,
        dataIndex:'device_ip'
      },{
        text:'分辨率',
        width:100,
        dataIndex:'device_resolution'
      },{
        text:'语言',
        width:75,
        dataIndex:'language'
      },{
        text:'上次登录',
        width:150,
        dataIndex:'last_login_time',
        renderer: datetime_renderer
      },{
        text:'系统版本',
        width:100,
        dataIndex:'os_version'
      }],
      listeners: {
        render: function(){}
      }
    });

    var appCombox = Ext.create('Ext.form.ComboBox', {
      fieldLabel: '应用',
      store: apps,
      editable: false,
      queryMode: 'local',
      displayField: 'label',
      valueField: 'app',
      listeners:{
        change: function(combobox,value){
          current_app = value;
          refresh();
        }
      }
    });

    var filelistPanel = Ext.create('Ext.grid.Panel',{
      store: fileStore,
      //width: WIDTH,
      height: HEIGHT-100,
      columns: [{
        text:'路径',
        width:200,
        dataIndex:'path'
      },{
        text:'修改时间',
        width:150,
        dataIndex:'mt',
        renderer:datetime_renderer
      },{
        text:'大小',
        width:100,
        dataIndex:'size',
        renderer:computersize_renderer
      }],
    });

    var filePanel = Ext.create('Ext.panel.Panel',{
      title: '文件',
      items: [appCombox,filelistPanel]
    });

    var messagePanel = Ext.create('Ext.grid.Panel',{
      store: messageStore,
      title: '消息',
      columns: [{
        text:'发出设备',
        width:300,
        dataIndex:'from_device',
        renderer:deviceid_renderer
      },{
        text:'接收设备',
        width:300,
        dataIndex:'to_device',
        renderer:deviceid_renderer
      },{
        text:'显示名称',
        width:300,
        dataIndex:'screen_name',
      },{
        text:'类型',
        width:100,
        dataIndex:'type',
      },{
        text:'路径',
        width:500,
        dataIndex:'path',
      }],
      listeners: {
        render: function(){
        }
      }
    });

    var userText = Ext.create('Ext.form.field.Text', {
      width: 300,
      name: 'user',
      fieldLabel: '用户ID',
      regex: /^[0-9]+$/
    });

    var current_tab_title = devicePanel.title;
    var current_app = false;

    function refresh(){
        var user = get_user();
        if(!user){
          return Ext.MessageBox.alert('警告','请输入正确的用户ID！');
        }
        switch(current_tab_title){
        case devicePanel.title:
          deviceAjax.url='cloudservice/api/devices?user='+user;
          deviceStore.removeAll(false);
          deviceStore.load();
          break;
        case filePanel.title:
          if(!current_app){
            return Ext.MessageBox.alert('警告','请选择应用！');
          }
          fileAjax.url = 'cloudservice/api/files?user='+user+'&app='+current_app;
          fileStore.removeAll(false);
          fileStore.load();
          break;
        case messagePanel.title:
          messageAjax.url = 'cloudservice/api/messages?user='+user;
          messageStore.removeAll(false);
          messageStore.load();
          break;
        }
    };

    var refreshButton = Ext.create('Ext.Button', {
      text: '刷新',
      handler: refresh
    });

    var tabPanel = Ext.create('Ext.tab.Panel',{
      height: HEIGHT,
      width: WIDTH,
      layout: 'card',
      items:[devicePanel,filePanel,messagePanel],
      listeners:{
        tabchange: function(panel,newcard,oldcard,opts){
          current_tab_title = newcard.title;
          if(current_tab_title!==filePanel.title){
            refresh();
          }
        }
      }
    });

    Ext.create('Ext.panel.Panel',{
      renderTo: Ext.getBody(),
      width: WIDTH,
      height: HEIGHT,
      title: 'Mx4 云服务查看页',
      layout: 'column',
      items: [
        userText,
        refreshButton,
        tabPanel
      ]
    });
  }
});

