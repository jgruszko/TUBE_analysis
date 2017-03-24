#!/bin/tcsh
foreach x (584 585 582 583 580 581 578 579 692 693 648 649 640 641 642 643 616 617 610 611 608 609 664 665 624 625 628 629 688 699 694 695 614 615 680 681 678 679 672 673 696 697 632 633 630 631 626 627 690 691 600 601 598 599 594 595 592 593)
     sed -e "s/Channels: 0/Channels: $x/g" -e "s/chan0/chan$x/g" < channel_0.dat > channel_$x.dat
     sed "s/channel_0/channel_$x/g" < submitAnalysis0 > submitAnalysis$x
     chmod +x submitAnalysis$x
     qsub submitAnalysis$x
end 


