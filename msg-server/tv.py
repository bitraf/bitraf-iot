#!/usr/bin/python
from bottle import route, run, template

@route('/message/<msg>')
def index(msg):
    content = """
<style>
    h1 {
    right:50%;
    bottom: 50%;
    transform: translate(50%,50%);
    position: absolute;
    font-size: 100px;
    font-family: Monospace;
    }
    </style>
    <h1>{{msg}}</h1>
"""

    return template(content, msg=msg)

run(host='tv.local', port=8080)
