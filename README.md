## SEGUIApp

**SEGUIApp** is an app for gathering data from stock exchanges, saving it to files and present a live view to the user. It also provides tools for modeling data that can be exported to CSV files and used to train machine learning models.


It requires *[SEFile](https://github.com/wdznak/SEFile)*, *[SESerivce](https://github.com/wdznak/SEService)* and *[Qt5](https://www.qt.io/)* to compile. GUI is based on Qt widget system.  




The main goal of this project is to test different approaches to design an app and improve my multithreading knowledge. In future releases, I'm going to implement a python module that will use gathered data to feed machine learning models.

Currently, the app is running on a Microsoft Azure virtual machine from 10/05/19 without any issues (day of the commit 30/05/19). 
![](../assets/app.png)


Data saved and compressed with 7zip



![](../assets/files.png)
