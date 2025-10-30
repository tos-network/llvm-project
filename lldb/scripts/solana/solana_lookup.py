from tos_providers import *
from tos_types import tosType, classify_tos_type


def summary_lookup(valobj, dict):
    # type: (SBValue, dict) -> str
    """Returns the summary provider for the given value"""
    tos_type = classify_tos_type(valobj.GetType())
    if tos_type == tosType.PUBKEY:
        return PubkeySummaryProvider(valobj, dict)
    if tos_type == tosType.ACCOUNT_INFO:
        return AccountInfoSummaryProvider(valobj, dict)
