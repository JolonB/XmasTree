import minify_html

def main(infile, outfile):
    # Open webui.html
    with open(infile, 'r') as f_in, open(outfile, 'w') as f_out:
        # Read file contents
        html = f_in.read()

        # Find <style> and </style> tag index
        style_start = html.find('<style>')
        style_end = html.find('</style>')

        # Replace any instances of % with %% between style_start and style_end
        html = html[:style_start] + html[style_start:style_end].replace('%', '%%') + html[style_end:]

        # Minify the file
        minified = minify_html.minify(html, minify_js=True, minify_css=True)

        # Surround string with rawliteral decorators
        minified = "R\"rawliteral(" + minified + ")rawliteral\""

        # Write the minified file
        f_out.write(minified)

if __name__ == "__main__":
    main('webui.html', 'webui.html.h')
