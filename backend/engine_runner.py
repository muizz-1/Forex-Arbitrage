import subprocess
import json
from pathlib import Path

from models import ArbitrageRequest


ROOT_DIR = Path(__file__).resolve().parent.parent

ENGINE_PATH = ROOT_DIR / "forex_cycles" / "build" / "forex_cycles"


def run_engine(req: ArbitrageRequest):

    # print("ENGINE PATH =", ENGINE_PATH)

    cmd = [
        str(ENGINE_PATH),
        *req.currencies,
        "--base", req.base_currency,
        "--amount", str(req.starting_amount),
        "--cost", str(req.tx_cost),
    ]

    if req.best_mode:
        cmd.append("--best")

    # print("CMD =", cmd)

    result = subprocess.run(
        cmd,
        capture_output=True,
        text=True,
        cwd=str(ROOT_DIR / "forex_cycles")
    )

    # print("STDOUT =", result.stdout)
    # print("STDERR =", result.stderr)

    output = result.stdout.strip()

    return json.loads(output)