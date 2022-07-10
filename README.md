BER-TLV is a data format defined by ISO/IEC 8825-1:2021. It's used widely in the payment industry for communicating data. 

BER stands for "Basic Encoding Rules" and TLV stands for "Tag", "Length, "Value".

The rules to parse the structure of BER-TLV is best described here:
Annex B of the EMV 4.3 Book 3 from EMVCo https://www.emvco.com/wp-content/uploads/2017/04/EMV_v4.3_Book_3_Application_Specification_20120607062110791.pdf

Here's an example:
9F020420005F2A03826

"Tag" 9F02 (Amount Paid)
"Length" 04
"Value" 2000

"Tag" 5F2A (Transaction Currency Code)
"Length" 03
"Value" 826 (GBP)

From this stream of data we can deduce the transaction was £20.00. 
