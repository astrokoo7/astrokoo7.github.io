# hello_world.rb

module Jekyll
    module HelloWorld
      def hello_world(input)
        "Hello World #{input}"
      end
    end
  end
  
  Liquid::Template.register_filter(Jekyll::HelloWorld)