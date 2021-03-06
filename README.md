## SEGUIApp

**Update 10/04/20** Project suspended.
Recently I added an offline mode to process data from files to calculate some statistics. The project detoured significantly from my initial idea and got a little bit messy, but I am happy with the overall modularity and ease of implementing new features. At this point, I did not learn new things by pushing it further so I decided to abandon it. At least for now. It was a fun project that thought me not only C++, Qt, design patterns but also Python and machine learning (Keras). 

**SEGUIApp** is an app for gathering data from stock exchanges, saving it to files and present a live view to the user. It also provides tools for modeling data that can be exported to CSV files and used to train machine learning models.


It requires *[SEFile](https://github.com/wdznak/SEFile)*, *[SESerivce](https://github.com/wdznak/SEService)* and *[Qt5](https://www.qt.io/)* to compile. GUI is based on Qt widget system.  




The main goal of this project is to test different approaches to design an app and improve my multithreading knowledge. In future releases, I'm going to implement a python module that will use gathered data to feed machine learning models.

Currently, the app is running on a Microsoft Azure virtual machine from 10/05/19 without any issues (day of the commit 30/05/19).
Edit 10/03/20. App is still up and have gathered over 200GB of data.
![](../assets/app.png)


Data saved and compressed with 7zip



![](../assets/files.png)








Offline mode



![](../assets/offmode.png)
