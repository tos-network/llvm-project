import re


class tosType(object):
    PUBKEY = "Pubkey"
    ACCOUNT_INFO = "AccountInfo"

PUBKEY_REGEX = re.compile(r"^(tos_program::pubkey::Pubkey)")
ACCOUNT_INFO_REGEX = re.compile(r"^(tos_program::account_info::AccountInfo)")

tos_TYPE_TO_REGEX = {
    tosType.PUBKEY: PUBKEY_REGEX,
    tosType.ACCOUNT_INFO: ACCOUNT_INFO_REGEX,
}

def classify_tos_type(type):
    for ty, regex in tos_TYPE_TO_REGEX.items():
        if regex.match(type.name):
            return ty
