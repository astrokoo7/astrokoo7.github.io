module Jekyll
    module SentenceFilter
      def extract_first_sentence(input)
        # first_sentence = input.split(/[.!?]/).first
        # "#{first_sentence}." unless first_sentence.nil?

        "???#{input}."
      end
    end
  end
  
  Liquid::Template.register_filter(Jekyll::SentenceFilter)