   M S B A M L   `   `   `  ����F  BFileCompare, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null;"clr-namespace:FileCompare.ControlsFileCompare.Controls  5   	   O Kmscorlib, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e0891&clr-namespace:System;assembly=mscorlibSystem 5   	   5clr-namespace:FileCompare.ModelFileCompare.Model  5   	   7 clr-namespace:FileCompare.CommonFileCompare.Common  5   	   )clr-namespace:FileCompareFileCompare  5	   	   R NWindowsBase, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35L5clr-namespace:System.Diagnostics;assembly=WindowsBaseSystem.Diagnostics 5
   	        FileCompare.FileComparator   5   	   /Controls"clr-namespace:FileCompare.Controls  \ XPresentationFramework, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35W SPresentationCore, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35D 9http://schemas.microsoft.com/winfx/2006/xaml/presentation    5   	   R NSystem.Xaml, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089:x,http://schemas.microsoft.com/winfx/2006/xaml     5   	   @d2http://schemas.microsoft.com/expression/blend/2008     5   	   Jmc;http://schemas.openxmlformats.org/markup-compatibility/2006     5   	   9System&clr-namespace:System;assembly=mscorlib     5   	   1Modelclr-namespace:FileCompare.Model     5   	   3Common clr-namespace:FileCompare.Common     5   	   +localclr-namespace:FileCompare     5	   	   Fdiag5clr-namespace:System.Diagnostics;assembly=WindowsBase     5
   	   $	��450��5   K   $	��728��6X   #��� ��6d     � Icon$  Images\app_big.ico��5   	    � Title 5   	   �� *DatabaseFile+ �� StringFormat$ File Compare - {0}�� � 	Resources 5       �� 
TargetType   LogViewerStyle �� Property �� Value K� IsItemsHost  DuplicatesBorder �� DataType    FileCompare.Model.LogEntry	 �� Padding&    FileCompare.Model.ItemDuplicates
 � IsSharedSizeScope � ColumnDefinitions z� SharedSizeGroup � RowDefinitions d� 	IsChecked �� Mode �� UpdateSourceTrigger��%�  &	        &	 �     (�� l    5(      *, +)7  (�� �    5-      *, +)�� 5   
   " ��6   .�5      ��  � �� 5      �� 5      .R�5      �� ��. .��5      �� 5      5      5      �� 5       � �� 5      �� 5      .R�5      $�  . 61   5      5!      5"      �� 5#   
   " ��6)   .�5$      ��  � ��$ 0 0 0 1q�60   �� 5%       � ��
 �����6,   5&      �� 5(   
   "  6   .R�5)      �� 8��  ._�5*      �� $	 3,1,0,0q�66   ��6   �� *
MainItem+66   5+      5,      �� 5-   
   "  6   .R�5.      � 
 .  5/      �� 50      $ checkbox��6'   $
��Auto �6B   �� 51      $ filepath��6'   $
��Auto �6B    53      �� 54      $
��Auto �6$   �� 55      .+�58      �� $��0��6    6+   �� *
IsSelected+$ TwoWay=�$ PropertyChanged=��� 59      $��1��6   $	 1,1,30,0q�6F   ��6,   �� *
FilePath+6F   5:      5;      5c      .��5e      ]� ��. .+�5f   
   `� $	��Top=�6    `� 
Background
 ���� .�5g      "   FileCompare.Controls.ToolBar  $
��Auto��6   $	��Top=�6,   %   OverflowToggleButtonBackground �	#00000000���	#00000000.@�5h      �� -   5         ��Name$ 	btnOpenDB��5h       � ToolTip$ Choose DB to open.��6,   .��5i      ��  �� Orientation$ 
Horizontal=�6!   .+�5j      �� $��Images\db_open.png��6    $��16��6<   �� 5k      $��3,0,3,0q�6$   $��Center=�65   Open6V   5l      5m      �� 5n      �� 5o      -   5        $ 
btnCleanDB��5o      $  Erase currently opened DB.��6P   ��6-   �� *EraseDBCommand+.��5p      �� $ 
Horizontal=�6!   .+�5q      �� $��Images\db_clean.png��6    $��16��6=   �� 5r      $��3,0,3,0q�6$   $��Center=�65   Erase6W   5s      5t      �� 5u      �� 5v      -   5        $ 
btnAnalyze��5v      $P JBrowse all contained folders, analyze included files and store info to DB.��6P   ��6-   �� *AnalyzeCommand+.��5w      �� $ 
Horizontal=�6!   .+�5x      �� $��Images\analyze_color.png��6    $��16��6B   �� 5y      $��3,0,3,0q�6$   $��Center=�65   	Analyze6Y   5z      5{      �� 5|      -   5        $ btnAbort��5|      $ Abort folder processing.��6L   ��6+   �� *AbortCommand+.��5}      �� $ 
Horizontal=�6!   .+�5~      �� $��Images\cancel.png��6    $��16��6;   �� 5      $��3,0,3,0q�6$   $��Center=�65   Abort6W   5�      5�      �� 5�      �� 5�      -   5        $ 
btnCompare��5�      $- 'Search DB for duplicates and list them.��6P   ��6-   �� *CompareCommand+.��5�      �� $ 
Horizontal=�6!   .+�5�      �� $��Images\compare.png��6    $��16��6<   �� 5�      $��3,0,3,0q�6$   $��Center=�65   	Compare6Y   5�      5�      5�      5�      ]� 5�   
   ��. $	��Top=�6)   $��8q�6>   .+�5�      �� $��Selected folder:��6   $��0,0,0,0q�6/   $
��Left=�6@   �� 5�      -   5        $ btnChangeFolder��5�      $��Change��6.   $��0,0,0,0q�6?   $��75��6P   $��Right=�6y   �� 5�      -   5         ��Name$ tbPath��5�      $��24��6g   $��Center=�6s   $��0,0,8,0q�6�    �� TextWrapping$ NoWrap=�6�   $��Center=�6�   ��6&   �� *
RootFolder+$ PropertyChanged=�6�   5�      �� 5�   
   $��Bottom=�6   ��6,   Z� ��5�      �� 5�      .R�5�      �  5�      �� 5�      $	��185 �6/   �� 5�      $
��auto �6/   �� 5�      $��* �6/   5�      5�      .@�5�      �� .��5�      �� -   5         ��Name$ labActivity��5�      $��Center=�6t   $��Stretch=�6�   $
��Left=�6�   ��6-   �� *StatusMessage+$ PropertyChanged=�6�   5�      �� 5�      $��1��6   $��3q�6(   #��� ��63   ��5�      �� 5�       �� Color# � ��6&   5�      �� 5�      $��2��6   $��Right=�6,   .��5�      �� -	   5         ��Name$ labSum��5�      $��3q�6,   $��Stretch=�67   $��Center=�6U    �� TextAlignment$ Right=�6p   $��20��6�   ��5�      `� 5�      $&  Found {0} folders and {1} files.��6'    `� UpdateSourceTrigger$ PropertyChanged=�6W   .1�5�      ��  �� Path$ FolderCount�6&   $ PropertyChanged=�69   �� 5�      $ 	FileCount�6&   $ PropertyChanged=�67   5�      5�      5�      5�      �� 5�   
   -
   5         ��Name$	 log��5�      $��8q�6l   #��[  6w   ��6$   �� *
LogEntries+$ PropertyChanged=���5�      �� 5�      .R�5�      �� ��. +   %FileCompare.Common.AutoScrollBehavior   
AutoScroll . #��� ��6g   $��3q�6�   .��5�      �� 5�      5�      ��5�      �� 5�      .R�5�      $�  . 6-   5�      5�      5�      5�      