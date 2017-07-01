# GOMfcTemplate2
      http://www.cnblogs.com/jsxyhelu/p/GOMFCTemplate2.html
      编写带界面的图像处理程序，选择opencv+mfc是一种很好的选择；在读取摄像头数据方面，
      网上的方法很多，其中shiqiyu的camerads的方法是较好的。
      基于现有资料，通过在实际项目中的积累，我总结出来一套结合opencv和mfc的摄像头采集框架。具有以下特点：
      1、基于directshow，兼容性好，速度快。到目前为止，无论是工业相机还是普通相机，没发现不兼容的；
      2、摄像头部分通过线程读取，保证界面的运行流畅；
      3、框架经过多次打磨，已经比较稳定，不会出现异常错误；代码简洁明了，方便复用。
 

