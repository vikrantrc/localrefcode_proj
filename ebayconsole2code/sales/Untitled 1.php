<?php

/**
 * @author 
 * @copyright 2013
 */
desired
SELECT CONCAT(STOCKV2.EAN, '-Import'), '-' as Title, 
(QTY1+QTY2+QTY3+QTY4+QTY5+QTY6+QTY7+QTY8) as Quantity, 
Price as USDMRP  , 20 as ‘DispatchTime’, 
Disc as Discount, 
ProductAvailabilityCode as Availability_20_21_22_are_avail, 
(STOCKV2.weight /220)+(STOCKV2.weight/1100) as weight 
 FROM STOCKV2,TTLINGV2 
 WHERE STOCKV2.EAN = TTLINGV2.EAN
 AND TTLINGV2.DESIRE is not null
 limit 0,300000



not desired
SELECT CONCAT(STOCKV2.EAN, '-Import'), '-' as Title, 
(QTY1+QTY2+QTY3+QTY4+QTY5+QTY6+QTY7+QTY8) as Quantity, 
Price as USDMRP  , 20 as ‘DispatchTime’, 
Disc as Discount, 
ProductAvailabilityCode as Availability_20_21_22_are_avail, 
(STOCKV2.weight /220)+(STOCKV2.weight/1100) as weight 
 FROM STOCKV2,TTLINGV2 
 WHERE STOCKV2.EAN = TTLINGV2.EAN
 AND TTLINGV2.DESIRE is  null
 limit 0,300000
?>