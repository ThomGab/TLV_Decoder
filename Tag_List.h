#ifndef TLV_Processing
	#include "TLV_Processing.h"
#endif

#ifndef Tag_List
	
	#define Tag_List

	#define SIZE_TAG_LIST 158

	static Tag TagList[SIZE_TAG_LIST] = {
		"4F", " - AID (Application Identifier)",
		"50", " - Application Label",
		"57", " - Track2 Equivalent Data",
		"5A", " - Application PAN",
		"5F20", " - Cardholder Name",
		"5F24", " - Application Expiration Date",
		"5F25", " - Application Effective Date",
		"5F28", " - Issuer Country Code",
		"5F2A", " - Transaction Currency Code",
		"5F2D", " - Language Preference",
		"5F30", " - Service Code",
		"5F34", " - Application PAN Sequence",
		"5F36", " - Transaction Currency Exponent",
		"5F57", " - Account Type",
		"61", " - Application Template",
		"6F", " - FCI (FILE CONTROL INFO) Template",
		"70", " - AEF (Application Elementary File) Data Template",
		"71", " - Issuer Script Template 1",
		"72", " - Issuer Script Template 2",
		"73", " - Directory Discretionary Template",
		"77", " - Response Message Template Format 2",
		"80", " - Response Message Template Format 1",
		"81", " - Amount Authorised Binary",
		"82", " - AIP (Application Interchange Profile)",
		"83", " - Command Template",
		"84", " - DF (Dedicated File) Name",
		"86", " - Issuer Script Command",
		"87", " - API (Application Priority Indicator)",
		"88", " - SFI (Short File Indicator)",
		"89", " - Authorisation Code",
		"8A", " - Authorisation Response Code",
		"8C", " - CDOL 1 (Card Risk Management Data Object List)",
		"8D", " - CDOL 2 (Card Risk Management Data Object List)",
		"8E", " - CVM (Cardholder Verification Method) List",
		"8F", " - Certification Authority Public Key Index",
		"90", " - Issuer Public Key Certificate",
		"91", " - Issuer Authentication Data",
		"92", " - Issuer Public Key Remainder",
		"93", " - Signed Static Application Data",
		"94", " - AFL (Application File Locator)",
		"95", " - TVR (Terminal Verification Results)",
		"97", " - TDOL (Transaction Certificate Data Object List)",
		"98", " - TC (Transaction Certificate) Hash Value",
		"99", " - Transaction PIN (Personal Identification Number) Data",
		"9A", " - Transaction Date",
		"9B", " - Transaction Status Info",
		"9C", " - Transaction Type",
		"9D", " - DDF (Directory Definition File) Name",
		"9F01", " - Acquirer Identifier",
		"9F02", " - Amount Authorised Numeric",
		"9F03", " - Amount Other Numeric",
		"9F04", " - Amount Other Binary",
		"9F05", " - Application Discretionary Data",
		"9F06", " - AID (Terminal)",
		"9F07", " - AUC (Application Usage Control)",
		"9F08", " - Application Version Number Card",
		"9F09", " - Application Version Number Terminal",
		"9F0A", " - Application Selection Registered Proprietary Data",
		"9F0B", " - Cardholder Name Extended",
		"9F0D", " - Issuer Action Code Default",
		"9F0E", " - Issuer Action Code Denial",
		"9F0F", " - Issuer Action Code Online",
		"9F10", " - Issuer Application Data",
		"9F11", " - Issuer Code Table Index",
		"9F12", " - Application Preferred Name",
		"9F13", " - Last Online ATC (Application Transaction Counter) Register",
		"9F14", " - Lower Consecutive Offline Limit",
		"9F15", " - Merchant Category Code",
		"9F16", " - Merchant Identifier",
		"9F17", " - PIN (Personal Identification Number) Try Counter",
		"9F18", " - Issuer Script Identifier",
		"5F50", " - Issuer URL",
		"9F1A", " - Terminal Country Code",
		"9F1B", " - Terminal Floor Limit",
		"9F1C", " - Terminal Identification",
		"9F1D", " - Terminal Risk Management Data",
		"9F1E", " - IFD (Interface Device) Serial Number",
		"9F1F", " - Track1 Discretionary Data",
		"9F20", " - Track2 Discretionary Data",
		"9F21", " - Transaction Time",
		"9F22", " - Certification Authority Public Key Index (Terminal)",
		"9F23", " - Upper Consecutive Offline Limit",
		"9F24", " - Payment Account Reference (PAR)",
		"9F26", " - Application Cryptogram",
		"9F27", " - Cryptogram Info Data",
		"9F2D", " - Icc PIN Encipherment Public Key Certificate",
		"9F2E", " - Icc PIN Encipherment Public Key Exponent",
		"9F2F", " - Icc PIN Encipherment Public Key Remainder",
		"9F32", " - Issuer Public Key Exponent",
		"9F33", " - Terminal Capabilities",
		"9F34", " - CVM (Cardholder Verification Method) Results",
		"9F35", " - Terminal Type",
		"9F36", " - ATC (Application Transaction Counter)",
		"9F37", " - Unpredicatable Number",
		"9F38", " - PDOL (Processing Options Data Object List)",
		"9F39", " - PoS (Point of Service) Entry Mode",
		"9F3A", " - Amount Reference Currency",
		"9F3B", " - Application Reference Currency",
		"9F3C", " - Transaction Reference Currency",
		"9F3D", " - Transaction Reference Currency Exponent",
		"9F40", " - Additional Terminal Capabilities",
		"9F41", " - Transaction Sequence Counter",
		"9F42", " - Application Currency Code",
		"9F43", " - Application Reference Currency Exponent",
		"9F44", " - Application Currency Exponent",
		"9F45", " - Data Authentication Code",
		"9F46", " - ICC Public Key Certificate",
		"9F47", " - ICC Public Key Exponent",
		"9F48", " - ICC Public Key Remainder",
		"9F49", " - DDOL (Dynamic Data Object List)",
		"9F4A", " - SDA (Static Data Authentication) Tag List",
		"9F4B", " - Signed Dynamic Application Data",
		"9F4C", " - ICC Dynamic Number",
		"9F4D", " - Log Entry",
		"9F4E", " - Merchant Name And Location",
		"9F4F", " - Log Format",
		"A5", " - FCI (File Control Info) Proprietary Template",
		"BF0C", " - FCI (File Control Info) Issuer Discretionary Data",
		"DF01", " - Terminal Action Code - Default",
		"DF02", " - Terminal Action Code - Denial",
		"DF03", " - Terminal Action Code - Online",
		"DF04", " - Issuer Public Key Modulus",
		"DF05", " - Issuer Hash Algorithm",
		"DF06", " - Issuer Sign Algorithm",
		"DF07", " - ICC Public Key Modulus",
		"DF08", " - ICC Hash Algorithm",
		"DF09", " - ICC Sign Algorithm",
		"DF0A", " - ICC PIN Encipherment Public Key Modulus",
		"DF0B", " - AUC Logic Override",
		"DF0C", " - CVM Process As Transaction Type",
		"DF0D", " - Issuer Certificate Recovered",
		"DF0E", " - ICC Certificate Recovered",
		"DF0F", " - SSAD Recovered",
		"DF10", " - SDAD Recovered",
		"DF11", " - User Zero Floor For Cashback",
		"DF12", " - Issuer Script Results",
		"DF13", " - Target Percentage to be used for Random Selection",
		"DF14", " - Maximum Target Percentage to be used for Random Selection",
		"DF15", " - Threshold Value for Random Selection",
		"DF16", " - CDA Hash Tags",
		"DF17", " - TAA Decline On Comms Failure (online only terminal)",
		"DF18", " - Force TRM to be performed irrespective of the AIP setting",
		"DF19", " - Issuer Scripts",
		"DF1A", " - Certification Authority Public Key Sign Algorithm",
		"DF1B", " - Certification Authority Public Key Hash Algorithm",
		"DF1C", " - Certification Authority Public Key Modulus",
		"DF1D", " - Certification Authority Public Key Exponent",
		"DF1E", " - Certification Authority Public Key Check Sum",
		"DF1F", " - Default CVM list",
		"DF20", " - CVM list to always be used instead of CVM list on the card",
		"DF21", " - Compare PAN And Track 2",
		"DF22", " - Additional Application Version Numbers supported by terminal",
		"DF23", " - Overwrite Terminal Application Version Number",
		"DF24", " - Application CVM Mask",
		"DF25", " - Decline If GenAC 1 Logic Error",
		"DF26", " - CDA Mode",
		"DF27", " - TRM Methods",
		"DF28", " - The result of ODA"
	};

#endif
